#include "IOR.h"
#include "IORUtil.h"

#include <bitset>
#include <QDateTime>
#include <QByteArray>
#include <unistd.h>
#include <QStandardPaths>

#include "Reply.h"
#include "Command.h"
#include "TimedState.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

IOR::IOR()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("ior/ip", "unknown").toString(),
                     settings_->value("ior/port", "9600").toInt(),
                     settings_->value("ior/connection_timeout", 3000).toInt(),
                     settings_->value("ior/read_timeout", 3000).toInt(),
                     settings_->value("ior/write_timeout", 3000).toInt(),
                     QByteArray("*\r\n")), // dont use cmd_footer_ since it has not yet been initialised. TODO: Find permanent solution.
                     reply_timeout_(settings_->value("ior/reply_timeout", "2000").toInt()),
                     ping_interval_(settings_->value("ior/ping_interval", 1000).toInt()),
                     logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    assert(QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray))));
    assert(QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(ConnectionEstablished()), this, SIGNAL(SIGNAL_Connected())));
    assert(QObject::connect(&socket_client_, SIGNAL(Disconnected()), this, SIGNAL(SIGNAL_Disconnected())));

    qDebug() << "IOR::IOR Reply timeout " << reply_timeout_;

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &IOR::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
}

void IOR::Ping() {
    // These three blocks contains all signals necessary for the VDF (CF, QZ, DT)
    ReadBlockValue(0, 0);
    ReadBlockValue(0, 1);
    ReadBlockValue(2, 1);
}

bool IOR::IsConnected() const {
    return socket_client_.Connected();
}

void IOR::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * cmd_queue_.size());
}

void IOR::StateMachineMessage(QString message) {
    qDebug() << "IOR::StateMachine " << QDateTime::currentDateTime() << " " << message;
}

void IOR::SetupStateMachine() {

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
                global::TimedState *sExecute = new global::TimedState(sExecuteParent, 600, "Pas de réponse, réessayer");

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
        QObject::connect(sConnected, &QState::entered, this, &ior::IOR::ClearQueue);
        QObject::connect(sConnected, &QState::entered, this, &ior::IOR::ReadAllChannels);
        QObject::connect(sConnected, &QState::entered, this, &ior::IOR::ReadAllMasks);

            // Idle
            QObject::connect(sIdle, &QState::entered, this, [&]() { if (!cmd_queue_.empty()) emit SIGNAL_CommandAdded(); });
            sIdle->addTransition(this, &ior::IOR::SIGNAL_CommandAdded, sExecuteParent);

            // Ping
            QObject::connect(sPing, &QState::entered, this, &ior::IOR::Ping);
            sPing->addTransition(sPing, &QState::entered, sIdle);
//            QObject::connect(sPing, &QState::entered, this, &ior::IOR::ProcessQueue);
//            sPing->addTransition(sPing, &QState::entered, sExecute);

            // ExecuteParent
            sExecuteParent->setInitialState(sExecute);
            QObject::connect(sExecuteParent, &global::TimedState::SIGNAL_Timeout, this, &IOR::WarningMessage);

                // Execute (Warning commented since timeouts happen every hour or so, but are not a problem)
                // QObject::connect(sExecute, &global::TimedState::SIGNAL_Timeout, this, &IOR::WarningMessage);
                QObject::connect(sExecute, &QState::entered, this, &ior::IOR::ProcessQueue);
                QSignalTransition *tReply = sExecute->addTransition(this, &ior::IOR::SIGNAL_ReplyReceived, sIdle);
                QObject::connect(tReply, &QSignalTransition::triggered, this, [&]() { active_cmd_.clear(); });
                sExecute->addTransition(this, &ior::IOR::SIGNAL_InterruptCommand, sError);
                sExecute->addTransition(this, &ior::IOR::SIGNAL_IOError, sDisconnected);

            // Error
            QObject::connect(sError, &QState::entered, this, &ior::IOR::ClearQueue);
            sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");
//    PrintStateChanges(sIdle, "Idle");
//    PrintStateChanges(sPing, "Ping");
//    PrintStateChanges(sExecute, "Execute");
    PrintStateChanges(sError, "Error");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void IOR::SetChannels(const std::vector<medicyc::cyclotroncontrolsystem::hardware::ior::Channel>& channels) {
    for  (Channel ch : channels) {
        channels_[ch.Name()] = ch;
        hashed_channels_[ior_util::Hash(ch.Card(), ch.Block(), ch.Address())] = ch;
    }
}

bool IOR::ChannelExist(QString name) const {
    return channels_.contains(name);
}

bool IOR::ChannelExist(int card, int block, int address) const {
    const int hash_value = ior_util::Hash(card, block, address);
    return hashed_channels_.contains(hash_value);
}

// CAN THROW
Channel IOR::GetChannel(QString name) const {
    if (!channels_.contains(name)) {
        throw std::runtime_error("No such channel");
    }
    return channels_.value(name);
}

// CAN THROW
Channel IOR::GetChannel(int card, int block, int address) const {
    const int hash_value = ior_util::Hash(card, block, address);
    if (!hashed_channels_.contains(hash_value)) {
        throw std::runtime_error("No such channel");
    }
    return hashed_channels_.value(hash_value);
}

void IOR::AddToQueue(medicyc::cyclotroncontrolsystem::hardware::ior::Command command) {
    if (!socket_client_.Connected()) {
        qWarning() << "IOR::AddToQueue Socket not connected, ignoring command";
        return;
    }
    if (cmd_queue_.size() >= MAXQUEUESIZE) {
        qWarning() << "IOR::AddToQueue Max queue size reached, ignoring command";
        return;
    }

    cmd_queue_.push(command);
    emit SIGNAL_CommandAdded();
}

void IOR::ProcessQueue() {
    if (active_cmd_.isEmpty()) {
        if (!cmd_queue_.empty()) {
            active_cmd_ = cmd_queue_.pop().Packaged();
            qDebug() << "IOR::ProcessQueue " << QDateTime::currentDateTime() << " " << active_cmd_;
            socket_client_.Write(active_cmd_);
        } else {
            qDebug() << "IOR::ProcessQueue Empty queue";
            DebugMessage("Plus de commandes dans la queue");
            emit SIGNAL_InterruptCommand();
        }
    } else {
        qDebug() << "IOR::ProcessQueue Resending " << QDateTime::currentDateTime() << " " << active_cmd_;
        socket_client_.Write(active_cmd_);
    }
}

void IOR::ClearQueue() {
    cmd_queue_.clear();
    active_cmd_.clear();
}

void IOR::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("IOR"), message);
}

void IOR::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("IOR"), message);
}

void IOR::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("IOR"), message);
}

void IOR::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("IOR"), message);
}

void IOR::Reset() {
    Command reset("$A", "0000", cmd_footer_);
    AddToQueue(reset);
}

void IOR::ReadAllUsedChannels() {
    for (int iCard = 0;  iCard < 3; ++iCard) {
        for (int iBlock = 0; iBlock < 3;  ++iBlock) {
            ReadBlockValue(iCard, iBlock);
        }
    }
}

void IOR::ReadAllChannels() {
    for (int iCard = 0;  iCard < 4; ++iCard) {
        for (int iBlock = 0; iBlock < 3;  ++iBlock) {
            ReadBlockValue(iCard, iBlock);
        }
    }
}

void IOR::ReadChannelValue(medicyc::cyclotroncontrolsystem::hardware::ior::Channel channel) {
    Command read_channel("$B", QString::number(channel.Card())
                         + QString::number(channel.Block())
                         + QString::number(channel.Address()) + "0", cmd_footer_);
    AddToQueue(read_channel);
}

void IOR::ReadChannelValue(QString name) {
    if (ChannelExist(name)) {
        ReadChannelValue(GetChannel(name));
    } else {
        QString message = QString("Signal inconnu: %1").arg(name);
        WarningMessage(message);
        emit SIGNAL_InterruptCommand();
    }
}

void IOR::ReadChannelMask(QString name) {
    if (ChannelExist(name)) {
        Channel ch = GetChannel(name);
        ReadBlockMask(ch.Card(), ch.Block());
    } else {
        QString message = QString("Signal inconnu: %1").arg(name);
        WarningMessage(message);
        emit SIGNAL_InterruptCommand();
    }
}

void IOR::ReadBlockValue(int card, int block) {
    Command cmd("$C", QString::number(card)
                         + QString::number(block)
                         + "00", cmd_footer_);
    AddToQueue(cmd);
}

void IOR::ReadBlockMask(int card, int block) {
    Command cmd("$D", QString::number(card)
                         + QString::number(block)
                         + "00", cmd_footer_);
    AddToQueue(cmd);
}

void IOR::ReadAllMasks() {
    for (int card = 0; card < NCARDS; ++card) {
       for (int block = 0; block < NBLOCKSPERCARD; ++block) {
            ReadBlockMask(card, block);
        }
    }
}

void IOR::SetCardAddressRange(int address_low, int address_high) {
    if (address_low < 0 || address_low > 7 ||
        address_high < 0 || address_high > 7 ||
        address_low >= address_high) {
        WarningMessage("Set card addresses corrupt: " +
                          QString::number(address_low) + " -> " +
                          QString::number(address_high));
        emit SIGNAL_InterruptCommand();
        return;
    }
    Command set_low_address("$F", "000" + QString::number(address_low), cmd_footer_);
    Command set_high_address("$G", "000" + QString::number(address_high), cmd_footer_);
    AddToQueue(set_low_address);
    AddToQueue(set_high_address);
}

void IOR::StartCyclicScan() {
    Command start("$H", "0000", cmd_footer_);
    AddToQueue(start);
}

void IOR::StopCyclicScan() {
    Command stop("$H", "0001", cmd_footer_);
    AddToQueue(stop);
}

void IOR::DoCompleteCyclicScans() {
    Command complete("$I", "0000", cmd_footer_);
    AddToQueue(complete);
}

void IOR::DoPartialCyclicScans(int address_first_card, int address_last_card) {
    if (address_first_card < 0 || address_first_card > 7 ||
        address_last_card < 0 || address_last_card > 7 ||
        address_first_card > address_last_card) {
        WarningMessage("Partial cyclic scan addresses corrupt: "
                          + QString::number(address_first_card) + " -> "
                          + QString::number(address_last_card));
        emit SIGNAL_InterruptCommand();
        return;
    }

    Command partial_low("$J", "000" + QString::number(address_first_card), cmd_footer_);
    Command partial_high("$K", "000" + QString::number(address_last_card), cmd_footer_);
    Command partial_enable("$I", "0001", cmd_footer_);
    AddToQueue(partial_low);
    AddToQueue(partial_high);
    AddToQueue(partial_enable);
}

void IOR::ReadConfiguration() {
    Command read_config("$L", "0002", cmd_footer_);
    AddToQueue(read_config);
}

void IOR::SetChannelTypeLAM() {
    Command cmd("$M", "0000", cmd_footer_);
    AddToQueue(cmd);
}

void IOR::SetBlockTypeLAM() {
    Command cmd("$M", "0001", cmd_footer_);
    AddToQueue(cmd);
}

void IOR::WriteBlockMask(int card, int block, quint8 mask) {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << mask;
    QString hex_mask = QString("%1").arg(QString(buffer.toHex()), 1, QLatin1Char('0'));
    Command write_mask("$N", QString::number(card) + QString::number(block) + hex_mask, cmd_footer_);
    AddToQueue(write_mask);
}

void IOR::ProcessIncomingData(QByteArray data) {
    QString data_string(data);
    Reply reply(data_string);
    qDebug() << "IOR::ProcessIncomingData " << QDateTime::currentDateTime() << " " << data_string;

    if (data_string.startsWith("$IOR")) {
        qDebug() << "IOR::ProcessIncomingData Ignoring message " << data;
        emit SIGNAL_ReplyReceived();
        return;
    }
    
    if (!reply.IsValid() || reply.Type() == ReplyType::UNKNOWN) {
        WarningMessage("Reçu un package de données inconnues: " + data);
        emit SIGNAL_InterruptCommand();
        return;
    } else {
        emit SIGNAL_ReplyReceived();
    }
    switch (reply.Type()) {
    case ReplyType::B:
        ProcessReadChannelReply(reply.Body());
        break;
    case ReplyType::C:
        ProcessReadBlockReply(reply.Body());
        break;
    case ReplyType::D:
        ProcessReadMaskReply(reply.Body());
        break;
    case ReplyType::E:
        ProcessErrorReply(reply.Body());
        break;
    case ReplyType::L:
        ProcessConfigReply(reply.Body());
        break;
    default:
        qDebug() << "IOR::ProcessIncomingData Ignored reply: " << data;
        break;
    }
}

void IOR::ProcessErrorReply(QString body) {
    if (body.size() != 4) {
        WarningMessage("Réponse d'erreur, la taille du message est incorrecte: '" + body + "'");
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int value(body.mid(2, 3).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse d'erreur, échec de la conversion en décimal: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    switch (value) {
    case 1:
        WarningMessage("Le matériel ior signale une erreur: Buffer full");
        emit SIGNAL_InterruptCommand();
        break;
    case 2:
        WarningMessage("Le matériel ior signale une erreur: Frame error");
        emit SIGNAL_InterruptCommand();
        break;
    case 3:
        WarningMessage("Le matériel ior signale une erreur: Overflow");
        emit SIGNAL_InterruptCommand();
        break;
    case 4:
        WarningMessage("Le matériel ior signale une erreur: Message invalid");
        emit SIGNAL_InterruptCommand();
        break;
    case 5:
        WarningMessage("Le matériel ior signale une erreur: Error checksum");
        emit SIGNAL_InterruptCommand();
        break;
    case 6:
        WarningMessage("Le matériel ior signale une erreur: Error configuration");
        emit SIGNAL_InterruptCommand();
        break;
    default:
        WarningMessage("Le matériel ior signale une erreur: Unknown error");
        emit SIGNAL_InterruptCommand();
        break;
    }
}

void IOR::ProcessConfigReply(QString body) {
    if (body.size() != 4) {
        WarningMessage("Réponse du config, la taille du message est incorrecte: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int value(body.mid(2, 2).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse du config, échec de la conversion en décimal: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    emit SIGNAL_IsConfigured((value & 64) > 0);
    emit SIGNAL_IsFormatChannel((value & 32) > 0);
    emit SIGNAL_ScanEnabled((value & 8) == 0);
    emit SIGNAL_FullScan((value & 16) == 0);
}

void IOR::ProcessReadChannelReply(QString body) {
    if (body.size() != 4) {
        WarningMessage("Réponse du lecture, la taille du message est incorrecte:  " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    const int card(body.left(1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse du lecture, échec de la conversion en décimal I: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    const int block(body.mid(1, 1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse du lecture, échec de la conversion en décimal II: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    const int address(body.mid(2, 1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse du lecture, échec de la conversion en décimal III: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    const int value(body.mid(3, 1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse du lecture, échec de la conversion en décimal IV: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    if (ChannelExist(card, block, address)) {
        Channel ch = GetChannel(card, block, address);
        emit SIGNAL_ReceivedChannelValue(ch.Name(), value > 0);
     } else {
         WarningMessage("Réponse du lecture, signal inconnu: " + body);
         emit SIGNAL_InterruptCommand();
    }
}

void IOR::ProcessReadBlockReply(QString body) {
    if (body.size() != 4) {
        WarningMessage("Réponse de lécture d'un block, la taille du message est incorrecte:  " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    int card(body.left(1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse de lécture d'un block, échec de la conversion en décimal I: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    int block(body.mid(1, 1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse de lécture d'un block, échec de la conversion en décimal II: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    int values(body.mid(2, 2).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse  de lécture d'un block, échec de la conversion en décimal III: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    if (ChannelExist(card, block, 0)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 0).Name(), (values & 1) > 0);
    if (ChannelExist(card, block, 1)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 1).Name(), (values & 2) > 0);
    if (ChannelExist(card, block, 2)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 2).Name(), (values & 4) > 0);
    if (ChannelExist(card, block, 3)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 3).Name(), (values & 8) > 0);
    if (ChannelExist(card, block, 4)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 4).Name(), (values & 16) > 0);
    if (ChannelExist(card, block, 5)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 5).Name(), (values & 32) > 0);
    if (ChannelExist(card, block, 6)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 6).Name(), (values & 64) > 0);
    if (ChannelExist(card, block, 7)) emit SIGNAL_ReceivedChannelValue(GetChannel(card, block, 7).Name(), (values & 128) > 0);
}

void IOR::ProcessReadMaskReply(QString body) {
    if (body.size() != 4) {
        WarningMessage("Lécture de mask, la taille du message est incorrecte:  " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    bool ok(false);
    const int card(body.left(1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse de mask, échec de la conversion en décimal I: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    const int block(body.mid(1, 1).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse de mask, échec de la conversion en décimal II: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }
    const int values(body.mid(2, 2).toInt(&ok, 16));
    if (!ok) {
        WarningMessage("Réponse de mask, échec de la conversion en décimal III: " + body);
        emit SIGNAL_InterruptCommand();
        return;
    }

    if (ChannelExist(card, block, 0)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 0).Name(), (values & 1) > 0);
    if (ChannelExist(card, block, 1)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 1).Name(), (values & 2) > 0);
    if (ChannelExist(card, block, 2)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 2).Name(), (values & 4) > 0);
    if (ChannelExist(card, block, 3)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 3).Name(), (values & 8) > 0);
    if (ChannelExist(card, block, 4)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 4).Name(), (values & 16) > 0);
    if (ChannelExist(card, block, 5)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 5).Name(), (values & 32) > 0);
    if (ChannelExist(card, block, 6)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 6).Name(), (values & 64) > 0);
    if (ChannelExist(card, block, 7)) emit SIGNAL_ReceivedChannelMask(GetChannel(card, block, 7).Name(), (values & 128) > 0);
}

void IOR::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "IOR " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "IOR " << QDateTime::currentDateTime() << " <-" << name; });
}

}
