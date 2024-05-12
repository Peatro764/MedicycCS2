#ifndef ADC_H
#define ADC_H

#include "IADC.h"
#include "ADC_global.h"

#include <QStateMachine>
#include <QObject>
#include <QString>
#include <vector>
#include <queue>
#include <memory>
#include <QSettings>
#include <QMap>

#include "SocketClient.h"
#include "Command.h"
#include "Channel.h"
#include "ThreadSafeQueue.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

namespace global = medicyc::cyclotroncontrolsystem::global;

class ADC_EXPORT ADC : public QObject, public IADC
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::adc::IADC)
public:
    ADC();
    void SetChannels(const std::vector<Channel>& channels);
    bool IsConnected() const;

public slots:
    void Ping() override;
    void Reset();
    void ReadConfiguration();
    void Read(const medicyc::cyclotroncontrolsystem::hardware::adc::Channel& channel);
    void Read(QString channel_name) override;
    void ReadAllChannels();

    void SetCardAddressRange(int address_low, int address_high);
    void StartCyclicScan();
    void StopCyclicScan();
    void DoCompleteCyclicScans();
    void DoPartialCyclicScans(int address_first_card, int address_last_card);

private slots:
    void ProcessIncomingData(QByteArray data);
    void StateMachineMessage(QString message);
    void AddToQueue(medicyc::cyclotroncontrolsystem::hardware::adc::Command command);
    void ProcessQueue();
    void ClearQueue();    
    void CheckIOLoad();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals:
    void SIGNAL_IOLoad(double load);
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_InterruptCommand();
    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived();

    void SIGNAL_IsConfigured(bool yes);
    void SIGNAL_ScanEnabled(bool yes);
    void SIGNAL_FullScan(bool yes);

    void SIGNAL_ReceivedChannelValue(medicyc::cyclotroncontrolsystem::hardware::adc::Channel channel, int raw_value);
    void SIGNAL_ReceivedChannelValue(QString channel, double physical_value, QString unit) override;

private:
    void SetupStateMachine();
    void ProcessErrorReply(QByteArray body);
    void ProcessConfigReply(QByteArray body);
    void ProcessReadChannelReply(QByteArray body);
    Channel GetChannel(int card, int address);
    bool ChannelExist(int card, int address) const;
    void PrintStateChanges(QState *state, QString name);

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    QMap<QString, Channel> channels_;
    QMap<int, Channel> hashed_channels_;
    const QByteArray command_footer_ = "*\r\n";
    global::ThreadSafeQueue<Command> command_queue_;
    int reply_timeout_ = 0;
    int ping_interval_ = 1000; // ms
    const size_t MAXQUEUESIZE = 50;
    QString active_cmd_= "";

    medicyc::cyclotron::MessageLoggerInterface logger_;

    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;
};

}

#endif // ADC_H
