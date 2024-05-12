#include "Omron.h"

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

Omron::Omron()
    : settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat))),
      socket_client_(settings_->value("omron_cyclo/ip", "unknown").toString(),
                     settings_->value("omron_cyclo/port", "9600").toInt(),
                     settings_->value("omron_cyclo/connection_timeout", 3000).toInt(),
                     settings_->value("omron_cyclo/read_timeout", 3000).toInt(),
                     settings_->value("omron_cyclo/write_timeout", 3000).toInt(),
                     QByteArray("")),
      fins_header_(static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_ICF", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_RSV", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_GCT", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_DNA", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_DA1", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_DA2", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_SNA", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_SA1", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_SA2", 0).toInt()),
                   static_cast<uint8_t>(settings_->value("omron_cyclo/fins_header_SID", 0).toInt())),
      reply_timeout_(settings_->value("omron_cyclo/reply_timeout", 2000).toInt()),
      ping_interval_(settings_->value("omron_cyclo/ping_interval", 3000).toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    assert(QObject::connect(&socket_client_, SIGNAL(DataRead(QByteArray)), this, SLOT(ProcessIncomingData(QByteArray))));
    assert(QObject::connect(&socket_client_, SIGNAL(ReadError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(WriteError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(PortError(QString)), this, SIGNAL(SIGNAL_IOError(QString))));
    assert(QObject::connect(&socket_client_, SIGNAL(ConnectionEstablished()), this, SIGNAL(SIGNAL_Connected())));
    assert(QObject::connect(&socket_client_, SIGNAL(Disconnected()), this, SIGNAL(SIGNAL_Disconnected())));
    assert(QObject::connect(this, &Omron::SIGNAL_InterruptCommand, this, &Omron::WarningMessage));

    ioload_calc_timer_.setInterval(200);
    ioload_calc_timer_.setSingleShot(false);
    QObject::connect(&ioload_calc_timer_, &QTimer::timeout, this, &Omron::CheckIOLoad);
    ioload_calc_timer_.start();

    ioload_emit_timer_.setInterval(2000);
    ioload_emit_timer_.setSingleShot(false);
    QObject::connect(&ioload_emit_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_IOLoad(io_load_); });
    ioload_emit_timer_.start();

    SetupStateMachine();
}

Omron::~Omron() {
    socket_client_.Disconnect();
}

bool Omron::IsConnected() const {
    return socket_client_.Connected();
}

void Omron::CheckIOLoad() {
    io_load_ = (0.9 * io_load_) + (0.1 * command_queue_.size());
}

void Omron::SetChannels(const QMap<QString, medicyc::cyclotroncontrolsystem::hardware::omron::Channel>& channels) {
    channels_ = channels;
    foreach(omron::Channel channel, channels.values()) {
        QString hash = omron::HashedAddress(channel.node(), channel.fins_memeory_area(), channel.address(), channel.bit());
        hashed_channels_[hash] = channel;
    }
}

void Omron::SetupStateMachine() {
    QState *sSuperState = new QState();
        global::TimedState *sDisconnected = new global::TimedState(sSuperState, 3*60000, "Toutes les tentatives de reconnexion ont échoué");
            global::TimedState *sRetryConnection = new global::TimedState(sDisconnected, 20000, "Retry");
        global::TimedState *sCheckConnection = new global::TimedState(sSuperState, 10000, "État de connexion inconnu", sDisconnected);
        QState *sConnected = new  QState(sSuperState);
        global::TimedState *sRequestNodeAddress = new global::TimedState(sConnected, 10000, "Échec de la demande d'adresse de nœud");
        QState *sInitalized = new QState(sConnected);
            QState *sPing = new QState(sInitalized); // currently not used
            global::TimedState *sIdle = new global::TimedState(sInitalized, ping_interval_, "Do Ping", sPing);
            QState *sError = new QState(sInitalized);
            global::TimedState *sExecute = new global::TimedState(sInitalized, reply_timeout_, "Pas de réponse de l'équipement", sDisconnected);

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
    QObject::connect(sConnected, &QState::entered, this, &Omron::ClearQueue);
    sConnected->setInitialState(sRequestNodeAddress);

        // RequestNodeAddress
        QObject::connect(sRequestNodeAddress, &QState::entered, this, &Omron::RequestNodeAddress);
        sRequestNodeAddress->addTransition(this, &Omron::SIGNAL_IOError, sDisconnected);
        QObject::connect(sRequestNodeAddress, &global::TimedState::SIGNAL_Timeout, this, &Omron::WarningMessage);
        sRequestNodeAddress->addTransition(this, &Omron::SIGNAL_NodeAddressReceived, sInitalized);

        // Initialized
        sInitalized->setInitialState(sIdle);

        // Idle
        QObject::connect(sIdle, &QState::entered, this, [&]() { if (!command_queue_.empty()) emit SIGNAL_CommandAdded(); });
        sIdle->addTransition(this, &omron::Omron::SIGNAL_CommandAdded, sExecute);

        // Ping
        QObject::connect(sPing, &QState::entered, this, [&](){ AddToQueue(new MemoryAreaRead(fins_header_, FINS_MEMORY_AREA::CIO, 100)); });
        sPing->addTransition(sPing, &QState::entered, sExecute);

        // Execute
        QObject::connect(sExecute, &global::TimedState::SIGNAL_Timeout, this, &Omron::WarningMessage);
        QObject::connect(sExecute, &QState::entered, this, &omron::Omron::PopQueue);
        QObject::connect(sExecute, &QState::exited, this, &omron::Omron::ClearPoppedCommand);
        sExecute->addTransition(this, &omron::Omron::SIGNAL_ReplyReceived, sIdle);
        sExecute->addTransition(this, &omron::Omron::SIGNAL_IOError, sDisconnected);
        sExecute->addTransition(this, &omron::Omron::SIGNAL_InterruptCommand, sError);

        // Error
        QObject::connect(sError, &QState::entered, this, &omron::Omron::ClearQueue);
        sError->addTransition(sError, &QState::entered, sIdle);

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sRetryConnection, "RetryConnection");
    PrintStateChanges(sCheckConnection, "CheckConnection");
    PrintStateChanges(sConnected, "Connection");

    // Start state machine
    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void Omron::AddToQueue(medicyc::cyclotroncontrolsystem::hardware::omron::MemoryAreaCommand* command) {
    if (!socket_client_.Connected()) {
        qWarning() << QDateTime::currentDateTime() << " Socket not connected, ignoring command";
        return;
    }
    if (command_queue_.size() < MAXQUEUESIZE) {
        command_queue_.push(command);
        qDebug() << QDateTime::currentDateTime() << " Added command on queue " << command->Command();
        emit SIGNAL_CommandAdded();
    } else {
        qWarning() << QDateTime::currentDateTime() << " Max queue size reached (" << MAXQUEUESIZE << "), ignoring command";
    }
}

void Omron::PopQueue() {
    if (!command_queue_.empty()) {
        popped_command_ = command_queue_.pop();
        socket_client_.Write(popped_command_->Command());
    } else {
        emit SIGNAL_InterruptCommand("Message queue is empty");
    }
}

void Omron::ClearQueue() {
    command_queue_.clear();
}

void Omron::ClearPoppedCommand() {
    if (popped_command_) {
        delete popped_command_;
        popped_command_ = nullptr;
    }
}

// COMMANDS

void Omron::RequestNodeAddress() {
    NodeAddressRequest cmd(fins_header_.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1));
    qDebug() << QDateTime::currentDateTime() << " Request node address";
    socket_client_.Write(cmd.Command());
}

// THROWS
Channel Omron::GetChannel(QString name) const {
    if (!channels_.contains(name)) {
        throw std::runtime_error("No such channel: " + name.toStdString());
    }
    return channels_.value(name);
}

// use this one before calling GetChannel
bool Omron::ChannelExist(QString name) const {
    return channels_.contains(name);
}

// THROWS
Channel Omron::GetChannel(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const {
    QString hash = omron::HashedAddress(node, memory_area, address, bit);
    if (!hashed_channels_.contains(hash)) {
        throw std::runtime_error("No such channel");
    }
    return hashed_channels_.value(hash);
}

// use this one before calling GetChannel
bool Omron::ChannelExist(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const {
    QString hash = omron::HashedAddress(node, memory_area, address, bit);
    return hashed_channels_.contains(hash);
}

// only bit implemented so far
void Omron::ReadChannel(QString name) {
    try {
        Channel ch = GetChannel(name);
        AddToQueue(new MemoryAreaRead(fins_header_.WithNewDA1(ch.node()) , ch.fins_memeory_area(), ch.address(), ch.bit()));
    } catch (std::exception& exc) {
        QString message = QString("ReadChannel threw an exception: ") + exc.what();
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    }
}

// only bit implemented so far
void Omron::WriteChannel(QString name, bool content) {
    try {
        Channel ch = GetChannel(name);
        if (ch.impuls()) {
            AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(ch.node()),  ch.fins_memeory_area(), ch.address(), ch.bit(), 0));
            AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(ch.node()),  ch.fins_memeory_area(), ch.address(), ch.bit(), 1));
            AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(ch.node()),  ch.fins_memeory_area(), ch.address(), ch.bit(), 0));
        } else {
            AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(ch.node()),  ch.fins_memeory_area(), ch.address(), ch.bit(), static_cast<uint16_t>(content)));
        }
    } catch (std::exception& exc) {
        QString message = QString("WriteChannel threw an exception: ") + exc.what();
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::ReadAllChannels() {
    try {
        foreach(omron::Channel ch, channels_.values()) {
            ReadBit(ch.node(), ch.fins_memeory_area(), ch.address(), ch.bit());
        }
    } catch (std::exception& exc) {
        QString message = QString("ReadAllChannels threw an exception: ") + exc.what();
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::Ping() {
    if (IsConnected()) {
        emit SIGNAL_Connected();
    } else {
        emit SIGNAL_Disconnected();
    }
}

void Omron::ReadWord(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address) {
    try {
        AddToQueue(new MemoryAreaRead(fins_header_.WithNewDA1(node), memory_area, address));
    } catch (std::exception& exc) {
        QString message = QString("ReadWord threw an exception: ") + exc.what();
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::ReadBit(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) {
    try {
    AddToQueue(new MemoryAreaRead(fins_header_.WithNewDA1(node), memory_area, address, bit));
    } catch (std::exception& exc) {
        QString message = QString("ReadBit threw an exception: ") + exc.what();
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::WriteWord(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content) {
    try {
    AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(node), memory_area, address, content));
    } catch (std::exception& exc) {
        QString message = QString("WriteWord threw an exception: ") + exc.what();
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::WriteBit(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, uint16_t content) {
    try {
    AddToQueue(new MemoryAreaWrite(fins_header_.WithNewDA1(node), memory_area, address, bit, content));
    } catch (std::exception& exc) {
        QString message = QString("WriteBit threw an exception: ") + exc.what();
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::ProcessIncomingData(QByteArray data) {
    try { // need to catch exceptions here since there is no way for higher levels to catch this exceptions otherwise,
          // since the action is initiated by the sending host

        qDebug() << QDateTime::currentDateTime() << " Received data " << data;

        if (!IsValidFINSReply(data)) {
            qDebug() << QDateTime::currentDateTime() << " Not a valid FINS reply";
            emit SIGNAL_InterruptCommand("Not a valid FINS reply");
            return;
        }

        FINS_COMMAND_TYPE cmd_type = ParseFINSReplyType(data);
        switch (cmd_type) {
        case FINS_COMMAND_TYPE::NODE_ADDRESS_RECV:
            ProcessNodeAddressReply(data);
            break;
        case FINS_COMMAND_TYPE::FRAME_SEND:
            ProcessFrameSendReply(data);
            break;
        case FINS_COMMAND_TYPE::FRAME_ERROR:
            ProcessFrameErrorReply(data);
            break;
        case FINS_COMMAND_TYPE::NODE_BUSY:
            ProcessNodeBusyReply();
            break;
        default:
            ProcessUnknownReply();
            break;
        }
    }
    catch (std::exception& exc) {
        QString message = QString("An exception was thrown when processing incoming data: ") + exc.what();
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    }
}

void Omron::ProcessUnknownReply() {
    QString message = QString("An unknown reply type was received.");
    qDebug() << QDateTime::currentDateTime() << " " << message;
    emit SIGNAL_InterruptCommand(message);
}

void Omron::ProcessNodeAddressReply(QByteArray data) {
    NodeAddressReply reply(data);
    const uint8_t client_node = reply.ClientNodeAddress();
    const uint8_t server_node = reply.ServerNodeAddress();
    qDebug() << QDateTime::currentDateTime() << " Client node address: " << QString::number(client_node);
    qDebug() << QDateTime::currentDateTime() << " Server node address: " << QString::number(server_node);
    // When writing to the internal network between Omron units, the server address should not be updated
    // since it is the LOCAL address within the network that should be used, not the external address
    //fins_header_.SetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1, server_node);
    fins_header_.SetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1, client_node);
    emit SIGNAL_NodeAddressReceived(fins_header_);
}

void Omron::ProcessFrameErrorReply(QByteArray data) {
    const uint32_t error_code = ParseFINSErrorCode(data);
    const QString message = QString("Frame Error ") + QString::number(error_code) + " closing connection";
    qDebug() << QDateTime::currentDateTime() << " " << message;
    emit SIGNAL_InterruptCommand(message);
}

void Omron::ProcessNodeBusyReply() {
    qDebug() << QDateTime::currentDateTime() << " Node address and IP occupied";
    emit SIGNAL_IOError(QString("Client node and IP already occupied"));
}

void Omron::ProcessFrameSendReply(QByteArray data) {
    const FINS_FRAME_SEND_COMMAND_TYPE type = ParseFINSFrameSendCommandType(data);
    switch (type) {
    case FINS_FRAME_SEND_COMMAND_TYPE::MEMORY_AREA_READ:
        ProcessMemoryReadReply(data);
        break;
    case FINS_FRAME_SEND_COMMAND_TYPE::MEMORY_AREA_WRITE:
        ProcessMemoryWriteReply(data);
        break;
    default:
        ProcessUnknownFrameSendReply();
        break;
    }
}

void Omron::ProcessUnknownFrameSendReply() {
    qDebug() << QDateTime::currentDateTime() << " Unknown frame send reply";
    emit SIGNAL_InterruptCommand("UnknownFrameSendReply");
}

void Omron::ProcessMemoryReadReply(QByteArray data) {
    qDebug() << QDateTime::currentDateTime() << " Recieved data is a memory read reply";
    if (!popped_command_) {
        emit SIGNAL_InterruptCommand("Received memory read reply but no active command");
        return;
    }

    popped_command_->DeserializeReply(data);

    if (popped_command_->MainResponseCode() != 0 || popped_command_->SubResponseCode() != 0) {
        QString message = QString("Reply error %1 %2").arg(popped_command_->MainResponseCode()).arg(popped_command_->SubResponseCode());
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    } else if (popped_command_->FrameError() != 0) {
        QString message = QString("Frame error %1").arg(popped_command_->FrameError());
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    } else {
        if (popped_command_->IsBitCommand()) {
            qDebug() << QDateTime::currentDateTime() << " Reply to a bit read command: Node " << popped_command_->Node()
                     << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                     << popped_command_->Address() << " Bit " << popped_command_->Bit() << " Content " << popped_command_->Content();
            emit SIGNAL_BitRead(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(),
                                popped_command_->Bit(), popped_command_->Content() > 0);
            if (ChannelExist(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Bit())) {
                emit SIGNAL_BitRead(GetChannel(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Bit()).name(),
                                     popped_command_->Content() > 0);
            } else {
                qDebug() << QDateTime::currentDateTime() << " Bit read command: Channel not in list: Node " << popped_command_->Node()
                         << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                         << popped_command_->Address() << " Content " << popped_command_->Content();
            }
        } else {
            emit SIGNAL_WordRead(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Content());
            qDebug() << QDateTime::currentDateTime() << " Reply to a word read command: Node " << popped_command_->Node()
                     << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                     << popped_command_->Address() << " Content " << popped_command_->Content();
            for (uint8_t bit = 0; bit < 16; ++bit) {
                const bool bit_content = (popped_command_->Content() & (1 << bit)) > 0;
                emit SIGNAL_BitRead(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(),
                                    bit, bit_content);
                if (ChannelExist(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), bit)) {
                    emit SIGNAL_BitRead(GetChannel(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), bit).name(), bit_content);
                } else {
                    qDebug() << QDateTime::currentDateTime() << " Word read command: Channel not in list: Node " << popped_command_->Node()
                             << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                             << popped_command_->Address() << " Content " << popped_command_->Content();
                }
            }
        }
    }
    emit SIGNAL_ReplyReceived();
}

void Omron::ProcessMemoryWriteReply(QByteArray data) {
    if (!popped_command_) {
        qDebug() << "No active command, but received this: " << data;
        emit SIGNAL_InterruptCommand("Received memory write reply but no active command");
        return;
    }
    popped_command_->DeserializeReply(data);

    if (popped_command_->MainResponseCode() != 0 || popped_command_->SubResponseCode() != 0) {
        QString message = QString("Reply error %1 %2").arg(popped_command_->MainResponseCode()).arg(popped_command_->SubResponseCode());
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    } else if (popped_command_->FrameError() != 0) {
        QString message = QString("Frame error %1").arg(popped_command_->FrameError());
        qDebug() << QDateTime::currentDateTime() << " " << message;
        emit SIGNAL_InterruptCommand(message);
    } else {
        if (popped_command_->IsBitCommand()) {
            qDebug() << QDateTime::currentDateTime() << " Reply to a bit write command: Node " << popped_command_->Node()
                     << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                     << popped_command_->Address() << " Bit " << popped_command_->Bit() << " Content " << popped_command_->Content();
            emit SIGNAL_BitWritten(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(),
                                   popped_command_->Bit(), popped_command_->Content() > 0);
            if (ChannelExist(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Bit())) {
                emit SIGNAL_BitWritten(GetChannel(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Bit()).name(),
                                     popped_command_->Content() > 0);
            }
        } else {
            qDebug() << QDateTime::currentDateTime() << " Reply to a word write command: Node " << popped_command_->Node()
                     << " MemoryArea " << static_cast<int>(popped_command_->MemoryArea()) << " Address "
                     << popped_command_->Address() << " Content " << popped_command_->Content();
            emit SIGNAL_WordWritten(popped_command_->Node(), popped_command_->MemoryArea(), popped_command_->Address(), popped_command_->Content());
        }
    }
    emit SIGNAL_ReplyReceived();
}

void Omron::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "OMRON " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "OMRON " << QDateTime::currentDateTime() << " <-" << name; });
}

void Omron::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Omron"), message);
}

void Omron::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Omron"), message);
}

void Omron::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Omron"), message);
}

void Omron::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Omron"), message);
}

}

