#include "ADC.h"
#include "ADCUtil.h"

#include "Reply.h"
#include "Command.h"
#include "TimedState.h"
#include "CountedTimedState.h"

#include <unistd.h>
#include <QStandardPaths>
#include <QDateTime>
#include <numeric>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

ADC::ADC()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("adc/ip", "unknown").toString(),
                     settings_->value("adc/port", "9600").toInt(),
                     settings_->value("adc/connection_timeout", 3000).toInt(),
                     settings_->value("adc/read_timeout", 3000).toInt(),
                     settings_->value("adc/write_timeout", 3000).toInt(),
                     QByteArray("*\r\n")), // dont use command_footer_ since it has not yet been initialised. TODO: Find permanent solution.
      reply_timeout_(settings_->value("adc/reply_timeout", "2000").toInt()),
      ping_interval_(settings_->value("adc/ping_interval", 3000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    QObject::connect(&socket_client_, &SocketClient::DataRead, this, &ADC::ProcessIncomingData);
    QObject::connect(&socket_client_, &SocketClient::ReadError, this, &ADC::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::WriteError, this, &ADC::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::PortError, this, &ADC::SIGNAL_IOError);
    QObject::connect(&socket_client_, &SocketClient::ConnectionEstablished, this, &ADC::SIGNAL_Connected);
    QObject::connect(&socket_client_, &SocketClient::Disconnected, this, &ADC::SIGNAL_Disconnected);

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &ADC::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
}

bool ADC::IsConnected() const {
    return socket_client_.Connected();
}

void ADC::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * command_queue_.size());
}

void ADC::StateMachineMessage(QString message) {
    qDebug() << QDateTime::currentDateTimeUtc().toString() << "ADC::StateMachine " << QDateTime::currentDateTime() << " " << message;
}

void ADC::SetupStateMachine() {

    // States

    QState *sSuperState = new QState();
        global::TimedState *sDisconnected = new global::TimedState(sSuperState, 3*60000, "Toutes les tentatives de reconnexion ont échoué");
            global::TimedState *sRetryConnection = new global::TimedState(sDisconnected, 20000, "Retry");
        global::TimedState *sCheckConnection = new global::TimedState(sSuperState, 10000, "État de connexion inconnu", sDisconnected);
    QState *sConnected = new QState(sSuperState);
        QState *sPing = new QState(sConnected);
        global::TimedState *sIdle = new global::TimedState(sConnected, ping_interval_, "Do Ping", sPing);
        QState *sError = new QState(sConnected);
        global::TimedState *sExecuteParent = new global::TimedState(sConnected, reply_timeout_, "Pas de réponse de l'équipement", sDisconnected);
            global::TimedState *sExecute = new global::TimedState(sExecuteParent, 500, "Pas de réponse, réessayer");

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
        QObject::connect(sConnected, &QState::entered, this, [&]() { InfoMessage("Connecté"); });
        sConnected->setInitialState(sIdle);
        QObject::connect(sConnected, &QState::entered, this, &adc::ADC::ClearQueue);

            // Idle
            QObject::connect(sIdle, &QState::entered, this, [&]() { if (!command_queue_.empty()) emit SIGNAL_CommandAdded(); });
            sIdle->addTransition(this, &adc::ADC::SIGNAL_CommandAdded, sExecuteParent);

            // Ping
            QObject::connect(sPing, &QState::entered, this, &adc::ADC::ReadConfiguration);
            sPing->addTransition(sPing, &QState::entered, sExecute);

            // ExecuteParent
            sExecuteParent->setInitialState(sExecute);
            QObject::connect(sExecuteParent, &global::TimedState::SIGNAL_Timeout, this, &ADC::DebugMessage);

                // Execute
                // QObject::connect(sExecute, &global::TimedState::SIGNAL_Timeout, this, &ADC::WarningMessage);
                QObject::connect(sExecute, &QState::entered, this, &adc::ADC::ProcessQueue);
                QSignalTransition *tReply = sExecute->addTransition(this, &adc::ADC::SIGNAL_ReplyReceived, sIdle);
                QObject::connect(tReply, &QSignalTransition::triggered, this, [&]() { active_cmd_.clear(); });
                sExecute->addTransition(this, &adc::ADC::SIGNAL_InterruptCommand, sError);
                sExecute->addTransition(this, &adc::ADC::SIGNAL_IOError, sDisconnected);

        // Error
        QObject::connect(sError, &QState::entered, this, &adc::ADC::ClearQueue);
        sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sPing, "Ping");
    PrintStateChanges(sExecute, "Execute");
    PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void ADC::SetChannels(const std::vector<Channel>& channels) {
    for  (const Channel &ch : channels) {
        channels_[ch.Name()] = ch;
        hashed_channels_[util::Hash(ch.Card(), ch.Address())] = ch;
    }
}

void ADC::AddToQueue(Command command) {
    if (!socket_client_.Connected()) {
        qWarning() << "ADC::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (command_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "ADC::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    command_queue_.push(command);
    emit SIGNAL_CommandAdded();
}

void ADC::ProcessQueue() {
    if (active_cmd_.isEmpty()) {
        if (!command_queue_.empty()) {
            active_cmd_ = command_queue_.pop().Packaged();
            qDebug() << "ADC::ProcessQueue " << QDateTime::currentDateTime() << " " << active_cmd_;
            socket_client_.Write(active_cmd_);
        } else {
            qDebug() << "ADC::ProcessQueue Empty queue";
            DebugMessage("Plus de commandes");
            emit SIGNAL_InterruptCommand();
        }
    } else {
        qDebug() << "ADC::ProcessQueue Resending " << QDateTime::currentDateTime() << " " << active_cmd_;
        socket_client_.Write(active_cmd_);
    }
}

void ADC::ClearQueue() {
    command_queue_.clear();
    active_cmd_.clear();
}

void ADC::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("ADC"), message);
}

void ADC::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("ADC"), message);
}

void ADC::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("ADC"), message);
}

void ADC::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("ADC"), message);
}

// COMMANDS

void ADC::Ping() {
    if (IsConnected()) {
        emit SIGNAL_Connected();
    } else {
        emit SIGNAL_Disconnected();
    }
}

void ADC::Reset() {
    Command reset("$A", "0000", command_footer_);
    AddToQueue(reset);
}

void ADC::ReadConfiguration() {
    Command read_config("$L", "0002", command_footer_);
    AddToQueue(read_config);
}

void ADC::ReadAllChannels() {
    for (auto& channel : channels_) {
        Read(channel);
    }
}

void ADC::Read(const Channel& channel) {
    QByteArray ch_hex = QString("%1").arg(channel.Address(), 1, 16, QLatin1Char('0')).toLatin1();
    Command read_channel("$U", QByteArray::number(channel.Card()) + ch_hex + "00", command_footer_);
    AddToQueue(read_channel);
}

void ADC::Read(QString channel_name) {
    if (!channels_.contains(channel_name)) {
        QString message = QString("Signal adc introuvable: %1").arg(channel_name);
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
    } else {
        Channel channel = channels_.value(channel_name);
        QByteArray ch_hex = QString("%1").arg(channel.Address(), 1, 16, QLatin1Char('0')).toLatin1();
        Command read_channel("$U", QByteArray::number(channel.Card()) + ch_hex + "00", command_footer_);
        AddToQueue(read_channel);
    }
}

// CAN THROW, CALL ChannelExist before using this function
Channel ADC::GetChannel(int card, int address) {
    const int hash_value = util::Hash(card, address);
    if (!hashed_channels_.contains(hash_value)) {
        qWarning() << "ADC::GetChannel The ADC channel does not exist " << card << " " << address;
        throw std::runtime_error("Signal adc introuvable");
    }
    return hashed_channels_.value(hash_value);
}

bool ADC::ChannelExist(int card, int address) const {
    const int hash_value = util::Hash(card, address);
    return hashed_channels_.contains(hash_value);
}

void ADC::SetCardAddressRange(int address_low, int address_high) {
    if (address_low < 0 || address_low > 3 ||
        address_high < 0 || address_high > 3 ||
        address_low >= address_high) {
        QString message = QString("Set card addresses corrupt: %1 %1").arg(address_low).arg(address_high);
        WarningMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }
    Command set_low_address("$F", "000" + QByteArray::number(address_low), command_footer_);
    Command set_high_address("$G", "000" + QByteArray::number(address_high), command_footer_);
    AddToQueue(set_low_address);
    AddToQueue(set_high_address);
}

void ADC::StartCyclicScan() {
    Command start("$H", "0000", command_footer_);
    AddToQueue(start);
}

void ADC::StopCyclicScan() {
    Command stop("$H", "0001", command_footer_);
    AddToQueue(stop);
}

void ADC::DoCompleteCyclicScans() {
    Command complete("$I", "0000", command_footer_);
    AddToQueue(complete);
}

void ADC::DoPartialCyclicScans(int address_first_card, int address_last_card) {
    if (address_first_card < 0 || address_first_card > 3 ||
        address_last_card < 0 || address_last_card > 3 ||
        address_first_card >= address_last_card) {
        QString message = QString("Partial cyclic scan addresses corrupt: %1 %1").arg(address_first_card).arg(address_last_card);
        WarningMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    Command partial_low("$J", "000" + QByteArray::number(address_first_card), command_footer_);
    Command partial_high("$K", "000" + QByteArray::number(address_last_card), command_footer_);
    Command partial_enable("$I", "0001", command_footer_);
    AddToQueue(partial_low);
    AddToQueue(partial_high);
    AddToQueue(partial_enable);
}

void ADC::ProcessIncomingData(QByteArray data) {
    qDebug() << "ADC::ProcessIncomingData Received " << data;
    Reply reply(data);

    if (data.startsWith("$ADC")) {
        qDebug() << "ADC::ProcessIncomingData Ignoring message " << data;
        emit SIGNAL_ReplyReceived();
        return;
    }

    if (!reply.IsValid() || reply.Type() == ReplyType::UNKNOWN) {
        QString message = QString("Reçu un package de données inconnues d'ADC: %1").arg(QString(data));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    } else {
        emit SIGNAL_ReplyReceived();
    }

    switch (reply.Type()) {
    case ReplyType::E:
        ProcessErrorReply(reply.Body());
        break;
    case ReplyType::L:
        ProcessConfigReply(reply.Body());
        break;
    case ReplyType::U:
        ProcessReadChannelReply(reply.Body());
        break;
    default:
        qDebug() << "ADC::ProcessIncomingData Ignored reply: " << data;
        break;
    }
}

void ADC::ProcessErrorReply(QByteArray body) {
    if (body.size() != 5) {
        QString message = QString("Réponse d'erreur, la taille du message est incorrecte: '" + QString(body) + "'");
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int value(body.mid(2, 3).toInt(&ok, 16));
    if (!ok) {
        QString message = QString("Réponse d'erreur, échec de la conversion en décimal: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    switch (value) {
    case 1:
        WarningMessage("Le matériel adc signale une erreur: Buffer full");
        emit SIGNAL_InterruptCommand();
        break;
    case 2:
        WarningMessage("Le matériel adc signale une erreur: Frame error");
        emit SIGNAL_InterruptCommand();
        break;
    case 3:
        WarningMessage("Le matériel adc signale une erreur: Overflow");
        emit SIGNAL_InterruptCommand();
        break;
    case 4:
        WarningMessage("Le matériel adc signale une erreur: Message invalid");
        emit SIGNAL_InterruptCommand();
        break;
    case 5:
        WarningMessage("Le matériel adc signale une erreur: Error checksum");
        emit SIGNAL_InterruptCommand();
        break;
    case 6:
        WarningMessage("Le matériel adc signale une erreur: Error configuration");
        emit SIGNAL_InterruptCommand();
        break;
    default:
        WarningMessage("Le matériel adc signale une erreur: Unknown error");
        emit SIGNAL_InterruptCommand();
        break;
    }
}

void ADC::ProcessConfigReply(QByteArray body) {
    if (body.size() != 5) {
        QString message = QString("Réponse du config, la taille du message est incorrecte: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int value(body.mid(3, 2).toInt(&ok, 16));
    if (!ok) {
        QString message = QString("Réponse du config, échec de la conversion en décimal: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    emit SIGNAL_IsConfigured((value & 64) > 0);
    emit SIGNAL_ScanEnabled((value & 8) == 0);
    emit SIGNAL_FullScan((value & 16) == 0);
}

void ADC::ProcessReadChannelReply(QByteArray body) {
    if (body.size() != 5) {
        QString message = QString("Réponse du lecture, la taille du message est incorrecte:  " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int card(body.left(1).toInt(&ok, 16));
    if (!ok) {
        QString message = QString("Réponse du lecture, échec de la conversion en décimal I: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }
    int address(body.mid(1, 1).toInt(&ok, 16));
    if (!ok) {
        QString message = QString("Réponse du lecture, échec de la conversion en décimal II: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }
    int value(body.mid(2, 3).toInt(&ok, 16));
    if (!ok) {
        QString message = QString("Réponse du lecture, échec de la conversion en décimal III: " + QString(body));
        DebugMessage(message);
        emit SIGNAL_InterruptCommand();
        return;
    }

    if (ChannelExist(card, address)) {
        Channel channel = GetChannel(card, address);
        emit SIGNAL_ReceivedChannelValue(channel, value);
        emit SIGNAL_ReceivedChannelValue(channel.Name(), channel.PhysicalValue(value), channel.Unit());
    } else {
        QString message = QString("Réponse du lecture, signal inconnu: Card %1 Addresse %2").arg(card).arg(address);
        WarningMessage(message);
        emit SIGNAL_InterruptCommand();
    }
}

void ADC::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "ADC " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "ADC " << QDateTime::currentDateTime() << " <-" << name; });
}


}
