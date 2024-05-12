#include "MultiplexedNSingle.h"

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

MultiplexedNSingle::MultiplexedNSingle(NSingleConfig nsingle_config, MultiplexConfig multiplex_config)
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      command_creator_(nsingle_config),
      reply_parser_(nsingle_config),
      socket_client_(nsingle_config.ip(), nsingle_config.port(),
                     settings_->value("nsingle/connection_timeout", 3000).toInt(),
                     settings_->value("nsingle/read_timeout", 3000).toInt(),
                     settings_->value("nsingle/write_timeout", 3000).toInt(),
                     QByteArray("*\r\n")),
      nsingle_config_(nsingle_config),
      multiplex_config_(multiplex_config),
      reply_timeout_(settings_->value("nsingle/reply_timeout", "2000").toInt()),
      reconnect_interval_(settings_->value("nsingle/reconnect_interval", 1000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    // The ANY channel is used to send a command to any channel: set and verify channel is thus not done
    multiplex_config_.AddChannel(ANY, 0);
    ConnectReplyParserSignals();
    ConnectSocketSignals();

    ioload_.reserve(20);
    ioload_timer_.setInterval(200);
    ioload_timer_.setSingleShot(false);
    QObject::connect(&ioload_timer_, &QTimer::timeout, this, &MultiplexedNSingle::CheckIOLoad);
    ioload_timer_.start();

    SetupStateMachine();
}

MultiplexedNSingle::~MultiplexedNSingle() {
    socket_client_.Disconnect();
}

void MultiplexedNSingle::CheckIOLoad() {
    const size_t s = ioload_.size();
    ioload_.push_back(cmd_queue_.size());
    if (s >= 10) ioload_.erase(ioload_.begin());
    const double mean = static_cast<double>(std::accumulate(ioload_.begin(), ioload_.end(), 0.0)) / static_cast<double>(s);
    emit SIGNAL_IOLoad(mean);
}

void MultiplexedNSingle::ConnectReplyParserSignals() {
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ConfigUpdated, this, [&](Config config) { emit SIGNAL_ConfigUpdated(active_nsingle_,  config); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ErrorUpdated, this, [&](Error error) { emit SIGNAL_ErrorUpdated(active_nsingle_, error); }); // This is a command error
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_StateUpdated, this, [&](StateReply state) { emit SIGNAL_StateUpdated(active_nsingle_, state); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_On, this, [&]() { if (active_nsingle_ == ANY) emit SIGNAL_On(); else emit SIGNAL_On(active_nsingle_); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Off, this, [&]() { if (active_nsingle_ == ANY) emit SIGNAL_Off(); else emit SIGNAL_Off(active_nsingle_); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Error, this, [&]() { if (active_nsingle_ == ANY) emit SIGNAL_Error(); else emit SIGNAL_Error(active_nsingle_); }); // This is the defaut signal on the nsingle
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Ok, this, [&]() { if (active_nsingle_ == ANY) emit SIGNAL_Ok(); else emit SIGNAL_Ok(active_nsingle_); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Local, this, [&]() { emit SIGNAL_Local(); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_Remote, this, [&]() { emit SIGNAL_Remote(); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_AddressValue, this, [&](const AddressRegister& address) { emit SIGNAL_AddressValue(active_nsingle_, address); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_AddressValue, this, &MultiplexedNSingle::VerifyCurrentNSingle);
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_AddressSetPoint, this, [&](const AddressRegister& address) { emit SIGNAL_AddressSetPoint(active_nsingle_, address); });
    QObject::connect(&reply_parser_, qOverload<double, bool>(&ReplyParser::SIGNAL_Channel1Value), this, [&](double physical_value, bool polarity) {
        emit SIGNAL_Channel1Value(physical_value, polarity); emit SIGNAL_Channel1Value(active_nsingle_, physical_value, polarity); });
    QObject::connect(&reply_parser_, qOverload<const Measurement&>(&ReplyParser::SIGNAL_Channel1Value), this, [&](const Measurement& m) { emit SIGNAL_Channel1Value(active_nsingle_, m); });
    QObject::connect(&reply_parser_, qOverload<double, bool>(&ReplyParser::SIGNAL_Channel1SetPoint), this, [&](double physical_value, bool polarity) {
        emit SIGNAL_Channel1SetPoint(physical_value, polarity); emit SIGNAL_Channel1SetPoint(active_nsingle_, physical_value, polarity); });
    QObject::connect(&reply_parser_, qOverload<const Measurement&>(&ReplyParser::SIGNAL_Channel1SetPoint), this, [&](const Measurement& m) { emit SIGNAL_Channel1SetPoint(active_nsingle_, m); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ParseError, this, [&](QString error) { emit SIGNAL_InterruptCommand(active_nsingle_, error); });
    QObject::connect(&reply_parser_, &ReplyParser::SIGNAL_ReplyReceived, this, &MultiplexedNSingle::SIGNAL_ReplyReceived);
    QObject::connect(this, &MultiplexedNSingle::SIGNAL_InterruptCommand, this, &MultiplexedNSingle::WarningMessage);
}

void MultiplexedNSingle::ConnectSocketSignals() {
    QObject::connect(&socket_client_, &SocketClient::DataRead, &reply_parser_, &ReplyParser::Parse);
    QObject::connect(&socket_client_, &SocketClient::ReadError, this, &MultiplexedNSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::WriteError, this, &MultiplexedNSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::PortError, this, &MultiplexedNSingle::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::ConnectionEstablished, this, &MultiplexedNSingle::SIGNAL_Connected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, &MultiplexedNSingle::SIGNAL_Disconnected);
}

void MultiplexedNSingle::IsConnected() {
    if (socket_client_.Connected()) {
        emit SIGNAL_Connected();
    } else {
        emit SIGNAL_Disconnected();
    }
}

// COMMANDS
void MultiplexedNSingle::On() {
    On(ANY);
}

void MultiplexedNSingle::On(QString name) {
    ChangeState(name, StateCommandType::On);
}

void MultiplexedNSingle::Off() {
    Off(ANY);
}

void MultiplexedNSingle::Off(QString name) {
    ChangeState(name, StateCommandType::Off);
}

void MultiplexedNSingle::Reset() {
    Reset(ANY);
}

void MultiplexedNSingle::Reset(QString name) {
    ChangeState(name, StateCommandType::Reset);
}

void MultiplexedNSingle::ChangeState(QString name, StateCommandType type) { try { AddToQueue(name, command_creator_.ChangeState(type)); }
                                                                            catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteAndVerifyAddressSetPoint(QString name, std::bitset<8> value) { try { AddToQueue(name, command_creator_.WriteAndVerifyAddressSetPoint(value)); }
                                                                                       catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteAddressSetPoint(QString name, std::bitset<8> value) { try { AddToQueue(name, command_creator_.WriteAddressSetPoint(value)); }
                                                                                       catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadAddressSetPoint(QString name) { try { AddToQueue(name, command_creator_.ReadAddressSetPoint()); }
                                                                catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadAddress(QString name) { try { AddToQueue(name, command_creator_.ReadAddress()); }
                                                        catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteChannel1SetPoint(QString name, int raw_value, bool sign) { try { AddToQueue(name, command_creator_.WriteChannel1SetPoint(raw_value, sign)); }
                                                                                         catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteAndVerifyChannel1SetPoint(QString name, const Measurement& m) { try { AddToQueue(name, command_creator_.WriteAndVerifyChannel1SetPoint(m)); }
                                                                                        catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteChannel1SetPoint(QString name, const Measurement& m) { try { AddToQueue(name, command_creator_.WriteChannel1SetPoint(m)); }
                                                                                        catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadChannel1SetPoint(QString name) { try { AddToQueue(name, command_creator_.ReadChannel1SetPoint()); }
                                                                 catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::Ping(QString name) { try { AddToQueue(name, command_creator_.Ping()); }
                                                         catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadChannel1(QString name) { try { AddToQueue(name, command_creator_.ReadChannel1()); }
                                                         catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadChannel1(QString name, int number) { try { AddToQueue(name, command_creator_.ReadChannel1(number)); }
                                                         catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::WriteChannel1Tolerance(QString name, std::bitset<16> value) { try { AddToQueue(name, command_creator_.WriteChannel1Tolerance(value)); }
                                                                                          catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::Write(QString name, Config config) { try { AddToQueue(name, command_creator_.Write(config)); }
                                                                 catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadState() { ReadState(ANY); }
void MultiplexedNSingle::ReadConfig() { ReadConfig(ANY); }
void MultiplexedNSingle::ReadError() { ReadError(ANY); }
void MultiplexedNSingle::ReadConfig(QString name) { try { AddToQueue(name, command_creator_.ReadConfig()); }
                                                       catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadState(QString name) { try { AddToQueue(name, command_creator_.ReadState()); }
                                                      catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};
void MultiplexedNSingle::ReadError(QString name) { try { AddToQueue(name, command_creator_.ReadError()); }
                                                      catch (std::exception& exc) { emit SIGNAL_InterruptCommand(name, exc.what()); }};

void MultiplexedNSingle::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
    QState *sDisconnected = new QState(sSuperState);
    QState *sBrokenConnection = new QState(sSuperState);
    QState *sConnected = new QState(sSuperState);
        QState *sIdle = new QState(sConnected);
        QState *sError = new QState(sConnected);
        global::TimedState *sExecute = new global::TimedState(sConnected, 4000, "Echec de l'exécution de la commande", sBrokenConnection);
            global::TimedState *sSelectNSingle = new global::TimedState(sExecute, 1000, "Echec de la sélection de nsingle");
            global::TimedState *sSendCommands = new global::TimedState(sExecute, 1000, "Echec de l'envoi de la commande");

    // SuperState
    sSuperState->setInitialState(sDisconnected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, [&]() { QTimer::singleShot(reconnect_interval_, this, SIGNAL(SIGNAL_Reconnect())); });
    sSuperState->addTransition(this, &MultiplexedNSingle::SIGNAL_Reconnect, sSuperState);

    // Disconnected
    QObject::connect(sDisconnected, &QState::entered, &socket_client_, &SocketClient::Connect);
    sDisconnected->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);

    // Broken connection
    QObject::connect(sBrokenConnection, &QState::entered, &socket_client_, &SocketClient::Disconnect);

    // Connected
    sConnected->setInitialState(sIdle);
    QObject::connect(sConnected, &QState::entered, this, &MultiplexedNSingle::ClearQueue);

        // Idle
        QObject::connect(sIdle, &QState::entered, this, [&]() { if (!cmd_queue_.empty()) emit SIGNAL_CommandAdded(); });
        sIdle->addTransition(this, &MultiplexedNSingle::SIGNAL_CommandAdded, sExecute);

        // Execute
        QObject::connect(sExecute, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(active_nsingle_, message); });
        QObject::connect(sExecute, &QState::entered, this, &MultiplexedNSingle::PopCommand);
        sExecute->addTransition(this, &MultiplexedNSingle::SIGNAL_InterruptCommand, sError);
        sExecute->addTransition(this, &MultiplexedNSingle::SIGNAL_IOError, sBrokenConnection);
        sExecute->addTransition(this, &MultiplexedNSingle::SIGNAL_MultiplexedCommandFinished, sIdle);
        sExecute->setInitialState(sSelectNSingle);

            // SelectNSingle
            QObject::connect(sSelectNSingle, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(active_nsingle_, message); });
            QObject::connect(sSelectNSingle, &QState::entered, this, &MultiplexedNSingle::SelectNSingle);
            sSelectNSingle->addTransition(this, &MultiplexedNSingle::SIGNAL_NSingleSuccessfullySelected, sSendCommands);

            // SendCommands
            QObject::connect(sSendCommands, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(active_nsingle_, message); });
            QObject::connect(sSendCommands, &QState::entered, this, &MultiplexedNSingle::ProcessQueue);
            sSendCommands->addTransition(&reply_parser_, &ReplyParser::SIGNAL_ReplyReceived, sSendCommands);
            sSendCommands->addTransition(this, &MultiplexedNSingle::SIGNAL_ExecutionFinished, sSendCommands);

    // Error
    sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sDisconnected, "Disonnected");
    PrintStateChanges(sConnected, "Connected");
//    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sBrokenConnection, "BrokenConnection");
//    PrintStateChanges(sExecute, "Execute");
//    PrintStateChanges(sSelectNSingle, "SelectNSingle");
//    PrintStateChanges(sSendCommands, "SendCommands");
    PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void MultiplexedNSingle::PopCommand() {
    if (cmd_queue_.empty()) {
        emit SIGNAL_InterruptCommand("MultiplexedNSingle", "La file d'attente de commandes est vide");
        qDebug() << "MultiplexedNSingle::ProcessQueue No command in queue. This can only be caused by a bug.";
        return;
    }
    active_command_ = cmd_queue_.pop();
}

void MultiplexedNSingle::SelectNSingle() {
    if (!multiplex_config_.ChannelExist(active_command_.nsingle())) {
        emit SIGNAL_InterruptCommand(active_command_.nsingle(), QString("NSingle multiplexé pas configuré. Commande ignorée."));
        return;
    }
    if (active_command_.nsingle() == ANY) {
        active_nsingle_ = ANY;
        emit SIGNAL_NSingleSuccessfullySelected();
        return;
    }
    Command write_address = command_creator_.WriteAddressSetPoint(multiplex_config_.SelectChannel(active_command_.nsingle()));
    socket_client_.Write(write_address.Packaged());
    Command read_address = command_creator_.ReadAddress();
    // Need to give the multiplexednsingle the time to actually change the channel (~100 ms)
    QTimer::singleShot(write_address.ExecutionTime(), this, [&, read_address]() {
        socket_client_.Write(read_address.Packaged());
    });
}

void MultiplexedNSingle::VerifyCurrentNSingle(const AddressRegister& address) {
    if (multiplex_config_.SelectChannel(active_command_.nsingle()) == address.bitset()) {
        active_nsingle_ = multiplex_config_.Name(address.bitset().to_ulong());
        emit SIGNAL_NSingleSuccessfullySelected();
    } else {
        qDebug() << QDateTime::currentDateTime() << "MultiplexedNSingle::VerifyCurrentNSingle Mismatch " << address.bitset().to_ulong() << " " << multiplex_config_.SelectChannel(active_command_.nsingle()).to_ulong();
        emit SIGNAL_NSingleSelectionMismatch();
    }
}

void MultiplexedNSingle::ProcessQueue() {
    if (active_command_.empty()) {
        emit SIGNAL_MultiplexedCommandFinished();
    } else {
        Command cmd = active_command_.pop();
        socket_client_.Write(cmd.Packaged());
        if (!cmd.Reply()) { QTimer::singleShot(cmd.ExecutionTime(), this, SIGNAL(SIGNAL_ExecutionFinished())); }
    }
}

void MultiplexedNSingle::AddToQueue(QString nsingle, Command cmd) {
    if (cmd_queue_.size() > MAXQUEUESIZE) {
        qWarning() << "MultiplexedNSingle::AddToQueue Max queue size reached, command ignored";
        return;
    }
    if (!multiplex_config_.ChannelExist(nsingle)) {
        WarningMessage(nsingle, QString("Nsingle multiplexé pas configuré. Commande ignorée."));
        return;
    }
    MultiplexedCommand m_cmd(nsingle, cmd);
    cmd_queue_.push(m_cmd);
    emit SIGNAL_CommandAdded();
}

void MultiplexedNSingle::AddToQueue(QString nsingle, std::queue<Command> cmds) {
    if (cmd_queue_.size() > MAXQUEUESIZE) {
        qWarning() << "MultiplexedNSingle::AddToQueue Max queue size reached, command ignored";
        return;
    }

    if (!multiplex_config_.ChannelExist(nsingle)) {
        WarningMessage(nsingle, QString("Nsingle multiplexé pas configuré. Commande ignorée."));
        return;
    }
    MultiplexedCommand m_cmd(nsingle, cmds);
    cmd_queue_.push(m_cmd);
    emit SIGNAL_CommandAdded();
}

void MultiplexedNSingle::ClearQueue() {
    cmd_queue_.clear();
}

void MultiplexedNSingle::WarningMessage(QString name, QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(name), message);
}

void MultiplexedNSingle::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "MultiplexedNSingle " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "MultiplexedNSingle " << QDateTime::currentDateTime() << " <-" << name; });
}

} // namespace
