#include "Barco.h"

#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <QDebug>
#include <QStandardPaths>

#include "MemoryAreaCommand.h"
#include "NodeAddressCommand.h"
#include "TimedState.h"
#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

Barco::Barco()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("omron_vide/ip", "unknown").toString(),
                     settings_->value("omron_vide/port", "9600").toInt(),
                     settings_->value("omron_vide/connection_timeout", 3000).toInt(),
                     settings_->value("omron_vide/read_timeout", 3000).toInt(),
                     settings_->value("omron_vide/write_timeout", 3000).toInt(),
                     QByteArray("")),
      fins_header_(static_cast<uint8_t>(settings_->value("omron_vide/fins_header_ICF", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_RSV", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_GCT", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_DNA", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_DA1", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_DA2", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_SNA", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_SA1", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_SA2", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_vide/fins_header_SID", 0).toInt())),
      reply_timeout_(settings_->value("omron_vide/reply_timeout", 2000).toInt()),
      ping_interval_(settings_->value("omron_vide/ping_interval", 3000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "Barco::Barco";
    assert(QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray))));
    assert(QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(ConnectionEstablished()), this, SIGNAL(SIGNAL_Connected())));
    assert(QObject::connect(&socket_client_, SIGNAL(Disconnected()), this, SIGNAL(SIGNAL_Disconnected())));

    SetupStateMachine();
}

Barco::~Barco() {
    socket_client_.Disconnect();
}

bool Barco::IsConnected() const {
    return socket_client_.Connected();
}

void Barco::SetMessages(QMap<int, medicyc::cyclotroncontrolsystem::hardware::omron::BarcoMessage> messages) {
    for (int id : messages.keys()) {
        qDebug() << id << " " << messages[id].message();
    }
    messages_ = messages;
}

void Barco::SetupStateMachine() {
    qDebug() << "Barco::SetupStateMachine";
    QState *sSuperState = new QState();
        global::TimedState *sDisconnected = new global::TimedState(sSuperState, 3*60000, "Toutes les tentatives de reconnexion ont échoué");
            global::TimedState *sRetryConnection = new global::TimedState(sDisconnected, 20000, "Retry");
        global::TimedState *sCheckConnection = new global::TimedState(sSuperState, 10000, "État de connexion inconnu", sDisconnected);
        QState *sConnected = new  QState(sSuperState);
            global::TimedState *sRequestNodeAddress = new global::TimedState(sConnected, 10000, "Échec de la demande d'adresse de nœud");
            QState *sIdle = new QState(sConnected);

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
    sConnected->setInitialState(sRequestNodeAddress);

    // RequestNodeAddress
    QObject::connect(sRequestNodeAddress, &QState::entered, this, &Barco::RequestNodeAddress);
    sRequestNodeAddress->addTransition(this, &omron::Barco::SIGNAL_IOError, sDisconnected);
    QObject::connect(sRequestNodeAddress, &global::TimedState::SIGNAL_Timeout, this, &Barco::WarningMessage);
    sRequestNodeAddress->addTransition(this, &Barco::SIGNAL_NodeAddressReceived, sIdle);

    // Idle
    // no more actions

    // Start state machine
    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

// COMMANDS

void Barco::RequestNodeAddress() {
    NodeAddressRequest cmd(fins_header_.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1));
    socket_client_.Write(cmd.Command());
}

void Barco::ProcessIncomingData(QByteArray data) {
    try { // need to catch exceptions here since there is no way for higher levels to catch this exceptions otherwise,
          // since the action is initiated by the sending host

        if (!IsValidFINSReply(data)) {
            qDebug() << "Barco::ProcessIncomingData Not a valid FINS reply " << data;
            throw std::runtime_error("Not a valid FINS reply");
        }

        FINS_COMMAND_TYPE cmd_type = ParseFINSReplyType(data);
        switch (cmd_type) {
        case FINS_COMMAND_TYPE::NODE_ADDRESS_RECV:
            ProcessNodeAddressReply(data);
            break;
        case FINS_COMMAND_TYPE::FRAME_SEND:
            ProcessFrameSendReply(data);
            break;
        default:
            qDebug() << "Barco::ProcessIncomingData Unknown commmand type " << static_cast<int>(cmd_type);
            throw std::runtime_error("Unknown reply type");
            break;
        }
    }
    catch (std::exception& exc) {
        qDebug() << QDateTime::currentDateTimeUtc().toString() << " Omron::ProcessIncomingData Exception: " << exc.what();
        QString message = QString("An exception was thrown when processing incoming data: ") + exc.what();
        WarningMessage(message);
        qDebug() << "Barco::ProcessIncomingData " << message;
    }
}

void Barco::ProcessNodeAddressReply(QByteArray data) {
    NodeAddressReply reply(data);
    const uint8_t client_node = reply.ClientNodeAddress();
    const uint8_t server_node = reply.ServerNodeAddress();
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " Barco::ProcessNodeAddressReply Client node address: " << QString::number(client_node);
    qDebug() << QDateTime::currentDateTimeUtc().toString() << " Barco::ProcessNodeAddressReply Server node address: " << QString::number(server_node);
    // When writing to the internal network between Omron units, the server address should not be updated
    // since it is the LOCAL address within the network that should be used, not the external address
    //fins_header_.SetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1, server_node);
    fins_header_.SetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1, client_node);
    emit SIGNAL_NodeAddressReceived(fins_header_);
}

void Barco::ProcessFrameSendReply(QByteArray data) {
    const FINS_FRAME_SEND_COMMAND_TYPE type = ParseFINSFrameSendCommandType(data);
    switch (type) {
    case FINS_FRAME_SEND_COMMAND_TYPE::MEMORY_AREA_WRITE:
        if (data.size() == 36) {
            ProcessBarcoMessage(static_cast<unsigned char>(data.at(data.size() - 1)) - 1);
        } else {
            qDebug() << "Barco::ProcessFrameSendReply Wrong size message " << data.size();
            WarningMessage("FrameSendReply packet size is not correct");
        }
        break;
    default:
        qDebug() << "Barco::ProcessFrameSendReply Unknown FrameSendReply type " << static_cast<int>(type);
        break;
    }
}

void Barco::ProcessBarcoMessage(int index) {
    qDebug() << "Barco::ProcessBarcoMessage " << index;
    if (messages_.contains(index)) {
        BarcoMessage msg = messages_[index];
        if (msg.enabled()) {
            qDebug() << "Barco::ProcessBarcoMessage " << messages_[index].message();
            switch (msg.severity()) {
            case BarcoMessage::SEVERITY::INFO:
                InfoMessage(msg.message());
                break;
            case BarcoMessage::SEVERITY::WARNING:
                WarningMessage(msg.message());
                break;
            case BarcoMessage::SEVERITY::ERROR:
                ErrorMessage(msg.message());
                break;
            default:
                qWarning() << "Barco::ProcessBarcoMessage Fell through switch";
            }
        } else {
            qDebug() << "Barco::ProcessBarcoMessage Message disabled: " << msg.message();
        }
    } else {
        qDebug() << "Barco::ProcessBarcoMessage Unknown message " << index;
    }
}

void Barco::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), SYSTEM, message);
}

void Barco::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), SYSTEM, message);
}

void Barco::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), SYSTEM, message);
}

void Barco::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), SYSTEM, message);
}

}

