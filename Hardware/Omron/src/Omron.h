#ifndef OMRON_H
#define OMRON_H

#include "IOmron.h"
#include "Omron_global.h"

#include <QObject>
#include <QString>
#include <QSettings>
#include <QStateMachine>
#include <QDateTime>
#include <memory>
#include <queue>
#include <QTimer>
#include <QMap>

#include "ThreadSafeQueue.h"
#include "Channel.h"
#include "SocketClient.h"
#include "FINS.h"
#include "MemoryAreaCommand.h"

#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

namespace global = medicyc::cyclotroncontrolsystem::global;

class OMRON_EXPORT Omron : public QObject, public IOmron
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::omron::IOmron)
public:
    Omron();
    ~Omron();
    bool IsConnected() const;
    void SetChannels(const QMap<QString, medicyc::cyclotroncontrolsystem::hardware::omron::Channel>& channels);

public slots:
    void Ping() override;
    void ReadWord(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address);
    void ReadBit(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit);
    void ReadChannel(QString name) override;
    void WriteWord(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
    void WriteBit(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, uint16_t content);
    void WriteChannel(QString name, bool content) override;
    void ReadAllChannels();

private slots:
    void RequestNodeAddress();

    void AddToQueue(medicyc::cyclotroncontrolsystem::hardware::omron::MemoryAreaCommand* command);
    void PopQueue();
    void ClearQueue();
    void CheckIOLoad();
    void ClearPoppedCommand();

    void ProcessIncomingData(QByteArray data);
    void ProcessNodeAddressReply(QByteArray data);
    void ProcessFrameSendReply(QByteArray data);
    void ProcessFrameErrorReply(QByteArray data);
    void ProcessNodeBusyReply();
    void ProcessUnknownReply();
    void ProcessMemoryReadReply(QByteArray data);
    void ProcessMemoryWriteReply(QByteArray data);
    void ProcessUnknownFrameSendReply();

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals:
    void SIGNAL_IOLoad(double load);
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_InterruptCommand(QString message);

    void SIGNAL_CommandAdded();
    void SIGNAL_NodeAddressReceived(medicyc::cyclotroncontrolsystem::hardware::omron::FINSHeader header);
    void SIGNAL_ReplyReceived();
    void SIGNAL_WordRead(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
    void SIGNAL_BitRead(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content);
    void SIGNAL_WordWritten(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint16_t content);
    void SIGNAL_BitWritten(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit, bool content);
    void SIGNAL_BitWritten(QString channel, bool content) override;
    void SIGNAL_BitRead(QString channel, bool content) override;


private:
    void SetupStateMachine();
    void PrintStateChanges(QState *state, QString message);
    medicyc::cyclotroncontrolsystem::hardware::omron::Channel GetChannel(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const;
    medicyc::cyclotroncontrolsystem::hardware::omron::Channel GetChannel(QString name) const;
    bool ChannelExist(uint8_t node, medicyc::cyclotroncontrolsystem::hardware::omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const;
    bool ChannelExist(QString name) const;

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    FINSHeader fins_header_;
    global::ThreadSafeQueue<MemoryAreaCommand*> command_queue_;
    MemoryAreaCommand* popped_command_ = nullptr;
    int reply_timeout_ = 0; // ms
    int ping_interval_ = 1000; // ms
    const size_t MAXQUEUESIZE = 50;

    QMap<QString, omron::Channel> channels_;
    QMap<QString, omron::Channel> hashed_channels_;

    medicyc::cyclotron::MessageLoggerInterface logger_;

    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;
};

}

#endif // OMRON_H
