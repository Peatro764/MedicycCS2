#include "Electrometer.h"

#include <unistd.h>
#include <QStandardPaths>
#include <QDateTime>
#include <numeric>
#include <QTimer>

#include "TimedState.h"
#include "CountedTimedState.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::electrometer {

Electrometer::Electrometer()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("electrometer/ip", "unknown").toString(),
                     settings_->value("electrometer/port", "1234").toInt(),
                     settings_->value("electrometer/connection_timeout", 3000).toInt(),
                     settings_->value("electrometer/read_timeout", 3000).toInt(),
                     settings_->value("electrometer/write_timeout", 3000).toInt(),
                     QByteArray("\n\r")), // dont use command_footer_ since it has not yet been initialised. TODO: Find permanent solution.
      reply_timeout_(settings_->value("electrometer/reply_timeout", 2000).toInt()),
      reconnect_interval_(settings_->value("electrometer/reconnect_interval", 1000).toInt()),
      max_leakage_current_(settings_->value("electrometer/max_leakage_current", 10e-12).toDouble()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray)));
//    QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));
//    QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));
    QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString)));
    QObject::connect(&socket_client_, SIGNAL(ConnectionEstablished()), this, SIGNAL(SIGNAL_Connected()));
    QObject::connect(&socket_client_, SIGNAL(Disconnected()), this, SIGNAL(SIGNAL_Disconnected()));
    QObject::connect(this, &Electrometer::SIGNAL_InterruptCommand, this, &Electrometer::WarningMessage);

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &Electrometer::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
}

bool Electrometer::IsConnected() const {
    return socket_client_.Connected();
}

void Electrometer::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * command_queue_.size());
}

void Electrometer::StateMachineMessage(QString message) {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << "Electrometer::StateMachine " << QDateTime::currentDateTime() << " " << message;
}

void Electrometer::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
    QState *sDisconnected = new QState(sSuperState);
    QState *sBrokenConnection = new QState(sSuperState);
    QState *sConnected = new QState(sSuperState);
        QState *sIdle = new QState(sConnected);
        QState *sInit = new QState(sConnected);
            global::TimedState *sWait = new global::TimedState(sInit, 2000, "Wait", sIdle);
            global::TimedState *sReadZeroCorr = new global::TimedState(sInit, 1000, "Failed reading zero correction", sBrokenConnection);
            global::TimedState *sReadZeroCheck = new global::TimedState(sInit, 1000, "Failed reading zero check", sBrokenConnection);
            global::TimedState *sReadFormElem = new global::TimedState(sInit, 1000, "Failed reading form element", sBrokenConnection);
            global::TimedState *sReadRange = new global::TimedState(sInit, 1000, "Failed reading range", sBrokenConnection);
            global::TimedState *sReadConfig = new global::TimedState(sInit, 1000, "Failed reading configuration", sBrokenConnection);

            global::TimedState *sSetZeroCorrOff = new global::TimedState(sInit, 500, "", sReadConfig);
            global::TimedState *sSetZeroCheckOff = new global::TimedState(sInit, 500, "", sSetZeroCorrOff);
            global::TimedState *sSetFormElem = new global::TimedState(sInit, 500, "", sSetZeroCorrOff);
            global::TimedState *sSetConfig = new global::TimedState(sInit, 500, "", sSetFormElem);
            global::TimedState *sSetPreset = new global::TimedState(sInit, 1000, "", sSetConfig);

            global::TimedState *sCheckAlive = new global::TimedState(sInit, 1000, "The electrometer does not respond", sBrokenConnection);

        global::CountedTimedState *sExecute = new global::CountedTimedState(sConnected, 8, "Plusieurs réponses manquantes, reconnexion a équipement", 3000, "Pas de réponse de l'équipement");
        QState *sError = new QState(sConnected);

    // SuperState
    sSuperState->setInitialState(sDisconnected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, [&]() { QTimer::singleShot(reconnect_interval_, this, SIGNAL(SIGNAL_Reconnect())); });
    sSuperState->addTransition(this, &Electrometer::SIGNAL_Reconnect, sSuperState);

        // Broken connection
        QObject::connect(sBrokenConnection, &QState::entered, &socket_client_, &SocketClient::Disconnect);

        // Disconnected
        QObject::connect(sDisconnected, &QState::entered, &socket_client_, &SocketClient::Connect);
        sDisconnected->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);

        // Connected
        sConnected->setInitialState(sInit);
        //sConnected->addTransition(this, &Electrometer::SIGNAL_Init, sInit);
        QObject::connect(sConnected, &QState::entered, this, &Electrometer::ClearQueue);

            // Init
            sInit->setInitialState(sCheckAlive);

            QObject::connect(sCheckAlive, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:ZCOR?" + command_footer_)); });
            sCheckAlive->addTransition(this, &Electrometer::SIGNAL_ReplyReceived, sSetPreset);

                // Write config
                QObject::connect(sSetPreset, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:PRES" + command_footer_)); });
                QObject::connect(sSetConfig, &QState::entered, this, [&]() { socket_client_.Write(QString("CONF:CURR" + command_footer_)); });
                QObject::connect(sSetFormElem, &QState::entered, this, [&]() { socket_client_.Write(QString("FORM:ELEM READ,TIME" + command_footer_)); });
                QObject::connect(sSetZeroCheckOff, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:ZCH 0" + command_footer_)); });
                QObject::connect(sSetZeroCorrOff, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:ZCOR 0" + command_footer_)); });

                // Read config
                QObject::connect(sReadConfig, &QState::entered, this, [&]() { socket_client_.Write(QString("CONF?" + command_footer_)); });
                sReadConfig->addTransition(this, &Electrometer::SIGNAL_ConfigCurrent, sReadRange);
                QObject::connect(sReadRange, &QState::entered, this, [&]() { socket_client_.Write(QString("SENS:CURR:RANGE?" + command_footer_)); });
                sReadRange->addTransition(this, &Electrometer::SIGNAL_CurrentRange, sReadFormElem);
                QObject::connect(sReadFormElem, &QState::entered, this, [&]() { socket_client_.Write(QString("FORM:ELEM?" + command_footer_)); });
                sReadFormElem->addTransition(this, &Electrometer::SIGNAL_ReadAndTimeFormElem, sReadZeroCheck);
                QObject::connect(sReadZeroCheck, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:ZCH?" + command_footer_)); });
                sReadZeroCheck->addTransition(this, &Electrometer::SIGNAL_ReplyZero, sReadZeroCorr);
                QObject::connect(sReadZeroCorr, &QState::entered, this, [&]() { socket_client_.Write(QString("SYST:ZCOR?" + command_footer_)); });
                sReadZeroCorr->addTransition(this, &Electrometer::SIGNAL_ReplyZero, sWait);

                // Idle
                sIdle->addTransition(this, &Electrometer::SIGNAL_CommandAdded, sExecute);
                QObject::connect(sIdle, &QState::entered, this, [&]() {
                if (command_queue_.empty()) {
                    ReadMeasurement();
                 } else {
                     emit SIGNAL_CommandAdded();
                }});

                // Execute
                QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_Timeout, this, &Electrometer::WarningMessage);
                QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, this, &Electrometer::ErrorMessage);
                QObject::connect(sExecute, &QState::entered, this, &Electrometer::PopQueue);
                QObject::connect(this, &Electrometer::SIGNAL_ReplyReceived, sExecute, &global::CountedTimedState::Reset);
                QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, sExecute, &global::CountedTimedState::Reset);
                QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_Timeout, sExecute, &global::CountedTimedState::Increment);
                sExecute->addTransition(sExecute, &global::CountedTimedState::SIGNAL_Timeout, sIdle);
                sExecute->addTransition(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, sBrokenConnection);
                sExecute->addTransition(this, &Electrometer::SIGNAL_ReplyReceived, sIdle);
                sExecute->addTransition(this, &Electrometer::SIGNAL_InterruptCommand, sError);
                sExecute->addTransition(this, &Electrometer::SIGNAL_IOError, sBrokenConnection);

    // Error
    QObject::connect(sError, &QState::entered, this, &Electrometer::ClearQueue);
    sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");

//    PrintStateChanges(sCheckAlive, "CheckAlive");
//    PrintStateChanges(sSetPreset, "SetPreset");
//    PrintStateChanges(sInit, "Init");
//    PrintStateChanges(sIdle, "Idle");
      PrintStateChanges(sBrokenConnection, "BrokenConnection");
//    PrintStateChanges(sExecute, "Execute");
      PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void Electrometer::AddToQueue(QString command) {
    if (!socket_client_.Connected()) {
        qWarning() << "Electrometer::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (command_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "Electrometer::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    command_queue_.push(command);
    emit SIGNAL_CommandAdded();
}

void Electrometer::PopQueue() {
    if (!command_queue_.empty()) {
        QString cmd = command_queue_.pop();
        socket_client_.Write(cmd + command_footer_);
        // if it doesnt contain a "?" no reply is coming -> force a signal to pass from execute to idle
        if (!cmd.contains('?')) { QTimer::singleShot(500, this,  SIGNAL(SIGNAL_ReplyReceived())); }
    } else {
        emit SIGNAL_InterruptCommand("Plus de commandes");
        qWarning() << "Electrometer::PopQueue Queue empty";
    }
}

void Electrometer::ClearQueue() {
    command_queue_.clear();
}

void Electrometer::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Electromètre Stripper"), message);
}

void Electrometer::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Electromètre Stripper"), message);
}

void Electrometer::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Electromètre Stripper"), message);
}

// COMMANDS

void Electrometer::Ping() {
    if (IsConnected()) {
        emit SIGNAL_Connected();
    } else {
        emit SIGNAL_Disconnected();
    }
}

void Electrometer::ReadAddress() {
    AddToQueue(GetReadAddressCommand());
}

void Electrometer::SetRange(double value) {
    qDebug() << "SetRange";
    if (std::abs(zero_) > 0.0) {
        emit SIGNAL_ZeroForcedRemoved();
        RemoveZero();
    }
    AddToQueue("SENS:CURR:RANGE " + QString::number(value, 'e', 2));
    QTimer::singleShot(500, this, [&](){ ReadRange(); });
}

void Electrometer::ReadRange() {
    qDebug() << "ReadRange";
    AddToQueue("SENS:CURR:RANGE?");
}

QString Electrometer::GetReadAddressCommand() const {
    return QString("++addr");
}

void Electrometer::SetAddress(int value) {
    AddToQueue(QString("++addr %1").arg(value));
}

void Electrometer::Init() {
    emit SIGNAL_Init();
}

void Electrometer::ReadMeasurement() {
    AddToQueue(QString("READ?"));
}

void Electrometer::ProcessIncomingData(QByteArray data) {
    emit SIGNAL_ReplyReceived();
    auto body = data.simplified();

    if (body == QString("READ,TIME")) {
        emit SIGNAL_ReadAndTimeFormElem();
        return;
    }

    if (body == QString("\"CURR:DC\"")) {
        emit SIGNAL_ConfigCurrent();
        return;
    }

    if (body == QString("0")) {
        emit SIGNAL_ReplyZero();
        return;
    }

    switch(body.size()) {
    case 2:
        ProcessAddressReply(body);
        break;
    case 13:
        ProcessRangeReply(body);
        break;
    case 27:
        ProcessMeasurement(body);
        break;
    default:
        break;
    }
}

void Electrometer::ProcessAddressReply(QByteArray body) {
    bool conv_ok(false);
    int address = body.toInt(&conv_ok);
    if (!conv_ok) {
        QString message = QString("Reçu un package de données inconnues du electrometer: %1").arg(QString(body));
        emit(SIGNAL_InterruptCommand(message));
        return;
    }
    emit SIGNAL_Address(address);
}

void Electrometer::ProcessMeasurement(QByteArray data) {
    QList<QByteArray> replies = data.split(',');
    if (replies.size() != 2) {
        emit SIGNAL_InterruptCommand("Measurement reply does not contain 2 items");
        return;
    }
    const double timestamp = replies.at(1).toDouble();
    const double value = replies.at(0).toDouble();
    const bool ol = (value > 0.95*range_);
    const double corrected_measurement = (value - zero_);
    emit SIGNAL_Measurement(timestamp,
                            corrected_measurement,
                            ol);
    // Ignore false readings. Current should never be above a few tens of uA, so a limuit of 1 A is ok
    if (abs(corrected_measurement) < 1.0) {
        AddToBuffer(corrected_measurement);
    } else {
        qDebug() << "Electrometer::ProcessMeasurement Out of bounds " << corrected_measurement;
    }
}

void Electrometer::AddToBuffer(double value) {
    measurement_.add(value);
    emit SIGNAL_Mean(measurement_.mean());
    emit SIGNAL_StdDev(measurement_.stddev());
}

void Electrometer::SetZero() {
   qDebug() << "SetZero";
   if (std::abs(measurement_.mean()) < max_leakage_current_) {
       zero_ = measurement_.mean();
       RAZ();
       emit SIGNAL_Zero(zero_);
   } else {
       emit SIGNAL_ErrorMessage(QString("Le courant de fuite trop haut\ncommande ignorée"));
   }
}

void Electrometer::RemoveZero() {
    qDebug() << "RemoveZero";
    zero_ = 0.0;
    RAZ();
    emit SIGNAL_ZeroRemoved();
}

void Electrometer::RAZ() {
    qDebug() << "RAZ";
    measurement_.reset();
    emit SIGNAL_Mean(measurement_.mean());
    emit SIGNAL_StdDev(measurement_.stddev());
}

void Electrometer::ProcessRangeReply(QByteArray data) {
    range_ = data.toDouble();
    emit SIGNAL_CurrentRange(range_);
    RAZ();
}


void Electrometer::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "Electrometer " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "Electrometer " << QDateTime::currentDateTime() << " <-" << name; });
}


}
