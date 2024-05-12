#ifndef IOR_H
#define IOR_H

#include "IOR_global.h"
#include "IIOR.h"
#include "IORUtil.h"
#include "Channel.h"

#include <QStateMachine>
#include <memory>
#include <QObject>
#include <QString>
#include <vector>
#include <QSettings>

#include "ThreadSafeQueue.h"
#include "SocketClient.h"
#include "Command.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

namespace global = medicyc::cyclotroncontrolsystem::global;

class IOR_EXPORT IOR : public QObject, public IIOR
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::ior::IIOR)
public:
    IOR();
    void Ping();
    bool IsConnected() const;
    void SetChannels(const std::vector<medicyc::cyclotroncontrolsystem::hardware::ior::Channel>& channels);

    int ncards() const { return NCARDS; }
    int ncolsperbit() const { return NCOLSPERBIT; }
    int nblockspercard() const { return NBLOCKSPERCARD; }
    int nbitsperblock() const { return NBITSPERBLOCK; }

public slots:
    void Reset();
    void ReadConfiguration();
    void ReadAllChannels();
    void ReadAllUsedChannels();
    void ReadAllMasks();
    void ReadChannelValue(medicyc::cyclotroncontrolsystem::hardware::ior::Channel channel);
    void ReadChannelValue(QString name) override;
    void ReadChannelMask(QString name) override;
    void ReadBlockMask(int card, int block);
    void ReadBlockValue(int card, int block);
    void WriteBlockMask(int card, int block, quint8 mask);

    void SetChannelTypeLAM();
    void SetBlockTypeLAM();
    void SetCardAddressRange(int address_low, int address_high);
    void StartCyclicScan();
    void StopCyclicScan();
    void DoCompleteCyclicScans();
    void DoPartialCyclicScans(int address_first_card, int address_last_card);

private slots:
    void ProcessIncomingData(QByteArray data);
    void StateMachineMessage(QString message);
    void AddToQueue(medicyc::cyclotroncontrolsystem::hardware::ior::Command command);
    void ProcessQueue();
    void ClearQueue();
    void CheckIOLoad();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals:
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_InterruptCommand();
    void SIGNAL_IOLoad(double load);

    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived();

    void SIGNAL_IsConfigured(bool yes);
    void SIGNAL_IsFormatChannel(bool yes);
    void SIGNAL_ScanEnabled(bool yes);
    void SIGNAL_FullScan(bool yes);

    void SIGNAL_ReceivedChannelValue(QString channel, bool value) override;
    void SIGNAL_ReceivedChannelMask(QString channel, bool mask) override;

private:
    void SetupStateMachine();
    void ProcessErrorReply(QString body);
    void ProcessConfigReply(QString body);
    void ProcessReadChannelReply(QString body);
    void ProcessReadBlockReply(QString body);
    void ProcessReadMaskReply(QString body);

    medicyc::cyclotroncontrolsystem::hardware::ior::Channel GetChannel(QString name) const;
    medicyc::cyclotroncontrolsystem::hardware::ior::Channel GetChannel(int card, int block, int address) const;
    bool ChannelExist(QString name) const;
    bool ChannelExist(int card, int block, int address) const;
    void PrintStateChanges(QState *state, QString name);

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    QMap<QString, Channel> channels_;
    QMap<int, Channel> hashed_channels_;
    global::ThreadSafeQueue<Command> cmd_queue_;
    const QString cmd_footer_ = "*\r\n";
    int reply_timeout_ = 0;
    int ping_interval_ = 1000; // ms
    QString active_cmd_= "";
    const size_t MAXQUEUESIZE = 50;
    medicyc::cyclotron::MessageLoggerInterface logger_;

    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;
};

}

#endif
