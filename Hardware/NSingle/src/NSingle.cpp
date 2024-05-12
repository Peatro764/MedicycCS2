#include "NSingle.h"

#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <QStandardPaths>
#include <QDateTime>

#include "Reply.h"
#include "TimedState.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

NSingle::NSingle(NSingleConfig config)
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      command_creator_(config),
      reply_parser_(config),
      socket_client_(config.ip(), config.port(),
                     settings_->value("nsingle/connection_timeout", 3000).toInt(),
                     settings_->value("nsingle/read_timeout", 3000).toInt(),
                     settings_->value("nsingle/write_timeout", 3000).toInt(),
                     QByteArray("*\r\n")),
      config_(config),
      reply_timeout_(settings_->value("nsingle/reply_timeout", "2000").toInt()),
      ping_interval_(settings_->value("nsingle/ping_interval", 3000).toInt()),
      reconnect_interval_(settings_->value("nsingle/reconnect_interval", 1000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    ConnectReplyParserSignals();
    ConnectSocketSignals();

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &NSingle::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
}

NSingle::~NSingle() {
    socket_client_.Disconnect();
}

void NSingle::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * cmd_queue_.size());
}

void NSingle::ConnectReplyParserSignals() {
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ConfigUpdated, this, &NSingle::SIGNAL_ConfigUpdated);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ErrorUpdated, this, &NSingle::SIGNAL_ErrorUpdated); // This is a command error
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_StateUpdated, this, &NSingle::SIGNAL_StateUpdated);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_On, this, &NSingle::SIGNAL_On);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Off, this, &NSingle::SIGNAL_Off);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Error, this, &NSingle::SIGNAL_Error); // This is the defaut signal on the nsingle
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Ok, this, &NSingle::SIGNAL_Ok);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Local, this, &NSingle::SIGNAL_Local);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Remote, this, &NSingle::SIGNAL_Remote);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_AddressValue, this, &NSingle::SIGNAL_AddressValue);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_AddressSetPoint, this, &NSingle::SIGNAL_AddressSetPoint);
    QObject::connect(&reply_parser_, qOverload<double, bool>(&ReplyParser::SIGNAL_Channel1Value), this, qOverload<double, bool>(&NSingle::SIGNAL_Channel1Value));
    QObject::connect(&reply_parser_, qOverload<const Measurement&>(&ReplyParser::SIGNAL_Channel1Value), this, qOverload<const Measurement&>(&NSingle::SIGNAL_Channel1Value));
    QObject::connect(&reply_parser_, qOverload<double, bool>(&ReplyParser::SIGNAL_Channel1SetPoint), this, qOverload<double, bool>(&NSingle::SIGNAL_Channel1SetPoint));
    QObject::connect(&reply_parser_, qOverload<const Measurement&>(&ReplyParser::SIGNAL_Channel1SetPoint), this, qOverload<const Measurement&>(&NSingle::SIGNAL_Channel1SetPoint));
    QObject::connect(&reply_parser_, qOverload<const Measurement&>(&ReplyParser::SIGNAL_Channel2Value), this, qOverload<const Measurement&>(&NSingle::SIGNAL_Channel2Value));
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ReplyReceived, this, &NSingle::SIGNAL_ReplyReceived);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ParseError, this, &NSingle::SIGNAL_InterruptCommand);
    QObject::connect(this, &NSingle::SIGNAL_InterruptCommand, this, &NSingle::WarningMessage);
}

void NSingle::ConnectSocketSignals() {
    QObject::connect(&socket_client_, &SocketClient::DataRead, &reply_parser_, &ReplyParser::Parse);
    QObject::connect(&socket_client_, &SocketClient::ReadError, this, &NSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::WriteError, this, &NSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::PortError, this, &NSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::ConnectionEstablished, this, &NSingle::SIGNAL_Connected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, &NSingle::SIGNAL_Disconnected);
}

void NSingle::IsConnected() {
    if (socket_client_.Connected()) {
        emit SIGNAL_Connected();
    } else {
        emit SIGNAL_Disconnected();
    }
}

// COMMANDS
void NSingle::Reset() { ChangeState(StateCommandType::Reset); }

void NSingle::ChangeState(StateCommandType type) { try { AddToQueue(command_creator_.ChangeState(type)); }
                                                   catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteAndVerifyAddressSetPoint(std::bitset<8> value) { try { AddToQueue(command_creator_.WriteAndVerifyAddressSetPoint(value)); }
                                                            catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteAddressSetPoint(std::bitset<8> value) { try { AddToQueue(command_creator_.WriteAddressSetPoint(value)); }
                                                            catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadAddressSetPoint() { try { AddToQueue(command_creator_.ReadAddressSetPoint()); }
                                       catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadAddress() { try{ AddToQueue(command_creator_.ReadAddress()); }
                               catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteAndVerifyChannel1SetPoint(const Measurement& m) {   try {
        AddToQueue(command_creator_.WriteAndVerifyChannel1SetPoint(m));
        qDebug() << "NSingle::WriteAndVerifyChannel1SetPoint HEX " << m.HexValue() << " Physical value " << m.InterpretedValue();
    } catch (std::exception& exc) {
        emit SIGNAL_InterruptCommand(exc.what());
    }
};
void NSingle::WriteChannel1SetPoint(const Measurement& m) {  try { AddToQueue(command_creator_.WriteChannel1SetPoint(m)); }
                                                             catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteChannel1SetPoint(int raw_value, bool sign) { try { AddToQueue(command_creator_.WriteChannel1SetPoint(raw_value, sign)); }
                                                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteChannel1SetPoint(double interpreted_value, bool sign) { try { AddToQueue(command_creator_.WriteChannel1SetPoint(interpreted_value, sign)); }
                                                                           catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteChannel1SetPoint(std::bitset<16> data) { try { AddToQueue(command_creator_.WriteChannel1SetPoint(data)); }
                                                            catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); }};
void NSingle::ReadChannel1SetPoint() { try { AddToQueue(command_creator_.ReadChannel1SetPoint()); }
                                        catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::Ping() { try { AddToQueue(command_creator_.Ping()); }
                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadChannel1() { try { AddToQueue(command_creator_.ReadChannel1()); }
                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadChannel1(int number) { try { AddToQueue(command_creator_.ReadChannel1(number)); }
                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::WriteChannel1Tolerance(std::bitset<16> value) { try { AddToQueue(command_creator_.WriteChannel1Tolerance(value)); }
                                                               catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadChannel2() { try { AddToQueue(command_creator_.ReadChannel2()); }
                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::Write(Config config) { try { AddToQueue(command_creator_.Write(config)); }
                                      catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadConfig() { try { AddToQueue(command_creator_.ReadConfig()); }
                              catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadState() { try { AddToQueue(command_creator_.ReadState()); }
                             catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };
void NSingle::ReadError() { try { AddToQueue(command_creator_.ReadError()); }
                             catch (std::exception& exc) { emit SIGNAL_InterruptCommand(exc.what()); } };

void NSingle::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
        QState *sBrokenConnection = new QState(sSuperState);
        QState *sDisconnected = new QState(sSuperState);
        QState *sConnected = new QState(sSuperState);
            QState *sPing = new QState(sConnected);
            global::TimedState *sIdle = new global::TimedState(sConnected, ping_interval_, "Do Ping", sPing);
            QState *sError = new QState(sConnected);
            global::TimedState *sExecute = new  global::TimedState(sConnected, reply_timeout_, "Pas de réponse de l'équipement", sBrokenConnection);

    // SuperState
    sSuperState->setInitialState(sDisconnected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, [&]() { QTimer::singleShot(reconnect_interval_, this, [&]() { emit SIGNAL_Reconnect(); }); });
    sSuperState->addTransition(this, &nsingle::NSingle::SIGNAL_Reconnect, sSuperState);

        // Broken connection
        QObject::connect(sBrokenConnection, &QState::entered, &socket_client_, &SocketClient::Disconnect);

        // Disconnected
        QObject::connect(sDisconnected, &QState::entered, &socket_client_, &SocketClient::Connect);
        sDisconnected->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);

        // Connected
        sConnected->setInitialState(sIdle);
        QObject::connect(sConnected, &QState::entered, this, &nsingle::NSingle::ClearQueue);

            // Idle
            QObject::connect(sIdle, &QState::entered, this, [&]() { if (!cmd_queue_.empty()) emit SIGNAL_CommandAdded(); });
            sIdle->addTransition(this, &nsingle::NSingle::SIGNAL_CommandAdded, sExecute);

            // Ping
            QObject::connect(sPing, &QState::entered, this,  [&]() { ReadState(); ReadChannel1(); });
            sPing->addTransition(sPing, &QState::entered, sExecute);

            // Execute
            QObject::connect(sExecute, &global::TimedState::SIGNAL_Timeout, this, &NSingle::WarningMessage);
            QObject::connect(sExecute, &QState::entered, this, &nsingle::NSingle::ProcessQueue);
            sExecute->addTransition(this, &nsingle::NSingle::SIGNAL_ReplyReceived, sIdle);
            sExecute->addTransition(this, &nsingle::NSingle::SIGNAL_ExecutionFinished, sIdle);
            sExecute->addTransition(this, &nsingle::NSingle::SIGNAL_InterruptCommand, sError);
            sExecute->addTransition(this, &nsingle::NSingle::SIGNAL_IOError, sBrokenConnection);

            // Error
            QObject::connect(sError, &QState::entered, this, &nsingle::NSingle::ClearQueue);
            sError->addTransition(sError, &QState::entered, sIdle);

//    PrintStateChanges(sDisconnected, "Disconnected");
//    PrintStateChanges(sConnected, "Connected");
//    PrintStateChanges(sIdle, "Idle");
//    PrintStateChanges(sPing, "Ping");
//    PrintStateChanges(sBrokenConnection, "BrokenConnection");
//    PrintStateChanges(sExecute, "Execute");
//    PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void NSingle::AddToQueue(Command command) {
    if (!socket_client_.Connected()) {
        qWarning() << "NSingle::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (cmd_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "NSingle::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    cmd_queue_.push(command);
    emit SIGNAL_CommandAdded();
}

void NSingle::AddToQueue(std::queue<Command> commands) {
    if (!socket_client_.Connected()) {
        qWarning() << "NSingle::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (cmd_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "NSingle::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    cmd_queue_.push(commands);
    emit SIGNAL_CommandAdded();
}

void NSingle::ProcessQueue() {
    if (!cmd_queue_.empty()) {
        Command cmd = cmd_queue_.pop();
        socket_client_.Write(cmd.Packaged());
        if (!cmd.Reply()) { QTimer::singleShot(cmd.ExecutionTime(), this, [&]() { emit SIGNAL_ExecutionFinished(); }); };
    } else {
        emit SIGNAL_InterruptCommand("Plus de commandes dans la file d'attente");
    }
}

void NSingle::ClearQueue() {
    cmd_queue_.clear();
}

void NSingle::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(config_.name()), message);
}

void NSingle::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(config_.name()), message);
}

void NSingle::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(config_.name()), message);
}

void NSingle::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "NSingle " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "NSingle " << QDateTime::currentDateTime() << " <-" << name; });
}


} // ns
