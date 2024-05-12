#include "SignalGenerator.h"

#include "TimedState.h"
#include "CountedTimedState.h"

#include <unistd.h>
#include <QStandardPaths>
#include <QDateTime>
#include <numeric>
#include <QTimer>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::signalgenerator {

SignalGenerator::SignalGenerator()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("signalgenerator/ip", "unknown").toString(),
                     settings_->value("signalgenerator/port", "1234").toInt(),
                     settings_->value("signalgenerator/connection_timeout", 3000).toInt(),
                     settings_->value("signalgenerator/read_timeout", 3000).toInt(),
                     settings_->value("signalgenerator/write_timeout", 3000).toInt(),
                     QByteArray("\r\n")), // dont use command_footer_ since it has not yet been initialised. TODO: Find permanent solution.
      frequency_(settings_->value("signalgenerator/frequency", 25.01).toDouble()),
      frequency_cold_(settings_->value("signalgenerator/frequency_cold", 25.01).toDouble()),
      frequency_warm_(settings_->value("signalgenerator/frequency_warm", 24.85).toDouble()),
      reply_timeout_(settings_->value("signalgenerator/reply_timeout", 2000).toInt()),
      ping_interval_(settings_->value("signalgenerator/ping_interval", 3000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    assert(QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray))));
    assert(QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(ConnectionEstablished()), this, SIGNAL(SIGNAL_Connected())));
    assert(QObject::connect(&socket_client_, SIGNAL(Disconnected()), this, SIGNAL(SIGNAL_Disconnected())));
    assert(QObject::connect(this, &SignalGenerator::SIGNAL_InterruptCommand, this, &SignalGenerator::WarningMessage));

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &SignalGenerator::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
    ConnectSignals();
}

void SignalGenerator::ConnectSignals() {
    QObject::connect(this, &SignalGenerator::SIGNAL_Frequency, this, [&](double frequency) {
        settings_->setValue("signalgenerator/frequency", QString::number(frequency, 'f', 3));
        settings_->sync();
        // Cold = starting frequency (25.01)
        // Warm = finished frequency (24.85 for protons)
        if (std::abs(frequency - frequency_cold_) < 0.00001) {
            qDebug() << "Frequency Cold";
            emit SIGNAL_FrequencyCold();
        }
        if (frequency - frequency_cold_ < -0.0001) {
            qDebug() << "Frequency below cold";
            emit SIGNAL_FrequencyBelowCold();
        }
        if (std::abs(frequency - frequency_warm_) < 0.00001) {
            qDebug() << "Frequency warm";
            emit SIGNAL_FrequencyWarm();
        }
        if (frequency - frequency_warm_ > 0.0001) {
            qDebug() << "Frequency above warm";
            emit SIGNAL_FrequencyAboveWarm();
        }
    });
}

bool SignalGenerator::IsConnected() const {
    return socket_client_.Connected();
}

void SignalGenerator::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * command_queue_.size());
}

void SignalGenerator::StateMachineMessage(QString message) {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << "SignalGenerator::StateMachine " << QDateTime::currentDateTime() << " " << message;
}

void SignalGenerator::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
        global::TimedState *sDisconnected = new global::TimedState(sSuperState, 3*60000, "Toutes les tentatives de reconnexion ont échoué");
            global::TimedState *sRetryConnection = new global::TimedState(sDisconnected, 20000, "Retry");
        global::TimedState *sCheckConnection = new global::TimedState(sSuperState, 10000, "État de connexion inconnu", sDisconnected);
    QState *sConnected = new QState(sSuperState);
        QState *sInit = new QState(sConnected);
        QState *sPing = new QState(sConnected);
        global::TimedState *sIdle = new global::TimedState(sConnected, ping_interval_, "Do Ping", sPing);
        QState *sError = new QState(sConnected);
        global::CountedTimedState *sExecute = new global::CountedTimedState(sConnected, 3, "Plusieurs réponses manquantes, reconnexion a équipement", reply_timeout_, "Pas de réponse de l'équipement");

    // Superstate
    sSuperState->setInitialState(sCheckConnection);

        // CheckConnection
        QObject::connect(sCheckConnection, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(message); });
        QObject::connect(sCheckConnection, &QState::entered, &socket_client_, &SocketClient::Connect);
        sCheckConnection->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);
        sCheckConnection->addTransition(&socket_client_, &SocketClient::Disconnected, sDisconnected);

        // Disconnected
        QObject::connect(sDisconnected, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { ErrorMessage(message); });
        QObject::connect(sDisconnected, &QState::entered, this, [&]() { WarningMessage("Déconnecté"); });
        QObject::connect(sDisconnected, &QState::entered, &socket_client_, &SocketClient::Disconnect);
        sDisconnected->setInitialState(sRetryConnection);

            // RetryConnection
            QObject::connect(sRetryConnection, &QState::entered, this, [&]() { InfoMessage("Essayer de se connecter"); });
            QObject::connect(sRetryConnection, &QState::entered, &socket_client_, &SocketClient::Connect);
            sRetryConnection->addTransition(&socket_client_, &SocketClient::ConnectionEstablished, sConnected);

        // Connected
        sConnected->setInitialState(sIdle);
        sConnected->addTransition(this, &SignalGenerator::SIGNAL_Init, sInit)->setTransitionType(QSignalTransition::InternalTransition);
        QObject::connect(sConnected, &QState::entered, this, &SignalGenerator::ClearQueue);

            // Init
            QObject::connect(sInit, &QState::entered, this, [&]() {
                AddToQueue("RL1DC1");
                SetFrequencyStep(FREQUENCY_STEP);
                SetFrequencyCold();
            });
            sInit->addTransition(sInit, &QState::entered, sIdle);

            // Idle
            QObject::connect(sIdle, &QState::entered, this, [&]() { if (!command_queue_.empty()) emit SIGNAL_CommandAdded(); });
            sIdle->addTransition(this, &SignalGenerator::SIGNAL_CommandAdded, sExecute);

            // Ping
            QObject::connect(sPing, &QState::entered, this, [&]() { if (IsConnected()) { emit SIGNAL_Frequency(frequency_); }});
            QObject::connect(sPing, &QState::entered, this, &SignalGenerator::ReadAddress);
            sPing->addTransition(sPing, &QState::entered, sExecute);

            // Execute
            QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_Timeout, this, &SignalGenerator::WarningMessage);
            QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, this, &SignalGenerator::ErrorMessage);
            QObject::connect(sExecute, &QState::entered, this, &SignalGenerator::PopQueue);
            QObject::connect(this, &SignalGenerator::SIGNAL_ReplyReceived, sExecute, &global::CountedTimedState::Reset);
            QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, sExecute, &global::CountedTimedState::Reset);
            QObject::connect(sExecute, &global::CountedTimedState::SIGNAL_Timeout, sExecute, &global::CountedTimedState::Increment);
            sExecute->addTransition(sExecute, &global::CountedTimedState::SIGNAL_Timeout, sIdle);
            sExecute->addTransition(sExecute, &global::CountedTimedState::SIGNAL_ThresholdReached, sDisconnected);
            sExecute->addTransition(this, &SignalGenerator::SIGNAL_ReplyReceived, sIdle);
            sExecute->addTransition(this, &SignalGenerator::SIGNAL_InterruptCommand, sError);
            sExecute->addTransition(this, &SignalGenerator::SIGNAL_IOError, sDisconnected);

    // Error
    QObject::connect(sError, &QState::entered, this, &SignalGenerator::ClearQueue);
    sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sInit, "Init");
//    PrintStateChanges(sPing, "Ping");
    PrintStateChanges(sExecute, "Execute");
    PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void SignalGenerator::AddToQueue(QString command) {
    if (!socket_client_.Connected()) {
        qWarning() << "SignalGenerator::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (command_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "SignalGenerator::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    command_queue_.push(command);
    emit SIGNAL_CommandAdded();
}

void SignalGenerator::PopQueue() {
    if (!command_queue_.empty()) {
        QString cmd = command_queue_.pop();
        if (cmd == GetReadAddressCommand()) {
            socket_client_.Write(cmd + command_footer_);
        } else {
            // This is to ensure that there is always a response.
            socket_client_.Write(cmd + command_footer_);
            socket_client_.Write(GetReadAddressCommand() + command_footer_);
        }
    } else {
        emit SIGNAL_InterruptCommand("Plus de commandes generateur frequence");
        qWarning() << "SignalGenerator::PopQueue Queue empty";
    }
}

void SignalGenerator::ClearQueue() {
    command_queue_.clear();
}

void SignalGenerator::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Generateur"), message);
}

void SignalGenerator::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Generateur"), message);
}

void SignalGenerator::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Generateur"), message);
}

// COMMANDS

void SignalGenerator::Ping() {
    if (IsConnected()) {
        emit SIGNAL_Connected();
        emit SIGNAL_Frequency(frequency_);
    } else {
        emit SIGNAL_Disconnected();
    }
}

void SignalGenerator::ReadAddress() {
    AddToQueue(GetReadAddressCommand());
}

QString SignalGenerator::GetReadAddressCommand() const {
    return QString("++addr");
}

void SignalGenerator::SetAddress(int value) {
    AddToQueue(QString("++addr %1").arg(value));
}

void SignalGenerator::Init() {
    qDebug() << "SignalGenerator::Init";
    emit SIGNAL_Init();
}

void SignalGenerator::SetAmplitude(double db) {
    if (IsConnected()) {
        AddToQueue(QString("AP%1DM").arg(db));
    } else {
        ErrorMessage("La commande est ignorée car la connexion au générateur est interrompue");
    }
}

void SignalGenerator::SetFrequency(double mhz) {
    if (!IsConnected()) {
        ErrorMessage("La commande est ignorée car la connexion au générateur est interrompue");
        return;
    }
    if (mhz > MAX_FREQUENCY || mhz < MIN_FREQUENCY) {
        ErrorMessage("Fréquence souhaitée en dehors des limites");
        return;
    }
    frequency_ = mhz;
    emit SIGNAL_Frequency(frequency_);
    AddToQueue(QString("FR%1MZ").arg(mhz));
}

void SignalGenerator::SetFrequencyCold() {
    SetFrequency(frequency_cold_);
}

void SignalGenerator::SetFrequencyStep(double mhz) {
    if (!IsConnected()) {
        ErrorMessage("La commande est ignorée car la connexion au générateur est interrompue");
        return;
    }
    AddToQueue(QString("FRIS%1MZ").arg(mhz));
}

void SignalGenerator::IncrementFrequency() {
    if (!IsConnected()) {
        ErrorMessage("La commande est ignorée car la connexion au générateur est interrompue");
        return;
    }
    if (frequency_ >= MAX_FREQUENCY) {
        ErrorMessage("Fréquence maximale atteinte");
        return;
    }
    frequency_ += FREQUENCY_STEP;
    emit SIGNAL_Frequency(frequency_);
    AddToQueue(QString("FRUP"));
}

void SignalGenerator::DecrementFrequency() {
    if (!IsConnected()) {
        ErrorMessage("La commande est ignorée car la connexion au générateur est interrompue");
        return;
    }
    if (frequency_ <= MIN_FREQUENCY) {
        ErrorMessage("Fréquence minimale atteinte");
        return;
    }
    frequency_ -= FREQUENCY_STEP;
    emit SIGNAL_Frequency(frequency_);
    AddToQueue(QString("FRDN"));
}

// At the moment the only data that is sent back is the address.
void SignalGenerator::ProcessIncomingData(QByteArray data) {
    emit SIGNAL_ReplyReceived();

    auto body = data.simplified();
    bool conv_ok(false);
    int address = body.toInt(&conv_ok);
    if (!conv_ok) {
        QString message = QString("Reçu un package de données inconnues du generateur: %1").arg(QString(data));
        emit SIGNAL_InterruptCommand(message);
        return;
    }
    emit SIGNAL_Address(address);
}

void SignalGenerator::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "SignalGenerator " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "SignalGenerator " << QDateTime::currentDateTime() << " <-" << name; });
}


}
