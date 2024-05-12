#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include "ISignalGenerator.h"
#include "SignalGenerator_global.h"

#include <QStateMachine>
#include <QObject>
#include <QString>
#include <vector>
#include <queue>
#include <memory>
#include <QSettings>
#include <QMap>

#include "SocketClient.h"
#include "ThreadSafeQueue.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::signalgenerator {

namespace global = medicyc::cyclotroncontrolsystem::global;

class SIGNALGENERATOR_EXPORT SignalGenerator : public QObject, public ISignalGenerator
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::signalgenerator::ISignalGenerator)
public:
    SignalGenerator();

public slots:
    void Ping();
    bool IsConnected() const;
    void ReadAddress();
    void SetAddress(int value);
    void Init();
    void SetAmplitude(double db);
    void SetFrequency(double mhz);
    void SetFrequencyCold();
    void SetFrequencyStep(double mhz);
    void IncrementFrequency();
    void DecrementFrequency();

private slots:
    void ProcessIncomingData(QByteArray data);
    void StateMachineMessage(QString message);
    void AddToQueue(QString command);
    void PopQueue();
    void ClearQueue();    
    void CheckIOLoad();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);

signals:
    void SIGNAL_Frequency(double mhz);
    void SIGNAL_FrequencyCold();
    void SIGNAL_FrequencyBelowCold();
    void SIGNAL_FrequencyWarm();
    void SIGNAL_FrequencyAboveWarm();
    void SIGNAL_IOLoad(double load);
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error);
    void SIGNAL_InterruptCommand(QString message);
    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived();
    void SIGNAL_Address(int address);
    void SIGNAL_Init();

private:
    void SetupStateMachine();
    void ProcessAddressReply(QByteArray body);
    void PrintStateChanges(QState *state, QString name);
    QString GetReadAddressCommand() const;
    void ConnectSignals();

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    const QByteArray command_footer_ = "\r";
    global::ThreadSafeQueue<QString> command_queue_;
    double frequency_ = 25.01;
    double frequency_cold_ = 25.01;
    double frequency_warm_ = 24.85;
    int reply_timeout_ = 0;
    int ping_interval_ = 1000; // ms
    const size_t MAXQUEUESIZE = 10;
    const double FREQUENCY_STEP = 0.001;
    const double MIN_FREQUENCY = 24.8;
    const double MAX_FREQUENCY = 25.1;
     medicyc::cyclotron::MessageLoggerInterface logger_;
    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;

};

}

#endif
