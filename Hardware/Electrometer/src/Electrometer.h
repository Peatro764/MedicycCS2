#ifndef Electrometer_H
#define Electrometer_H

#include "IElectrometer.h"
#include "Electrometer_global.h"

#include <QStateMachine>
#include <QObject>
#include <QString>
#include <vector>
#include <queue>
#include <memory>
#include <QSettings>
#include <QMap>
#include <math.h>

#include "SocketClient.h"
#include "ThreadSafeQueue.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::electrometer {

namespace global = medicyc::cyclotroncontrolsystem::global;

class Measurement {
public:
    float mean() { return sum_ / n_; }
    float stddev() {
        return sqrt((sum_squared_ / n_) - (mean()*mean())); }
    void add(float value) { n_++; sum_ += value; sum_squared_ += value*value; }
    void reset() { n_ = 0; sum_ = 0.0; sum_squared_ = 0.0; }
private:
    int n_ = 0;
    double sum_ = 0.0;
    double sum_squared_ = 0.0;
};

class ELECTROMETER_EXPORT Electrometer : public QObject, public IElectrometer
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::electrometer::IElectrometer)
public:
    Electrometer();

    bool IsConnected() const;

public slots:
    void Ping();
    void ReadAddress();
    void SetAddress(int value);
    void Init();
    void ReadMeasurement();
    void SetRange(double value); // dbus
    void ReadRange(); // dbus
    void SetZero(); // dbus
    void RemoveZero(); // dbus
    void RAZ(); // dbus

private slots:
    void ProcessIncomingData(QByteArray data);
    void ProcessMeasurement(QByteArray data);
    void ProcessRangeReply(QByteArray data);
    void StateMachineMessage(QString message);
    void AddToQueue(QString command);
    void PopQueue();
    void ClearQueue();    
    void CheckIOLoad();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void AddToBuffer(double value);

signals:
    void SIGNAL_ErrorMessage(QString message); // dbus
    void SIGNAL_IOLoad(double load);
    void SIGNAL_Connected() override; // dbus
    void SIGNAL_Disconnected() override; // dbus
    void SIGNAL_Reconnect();
    void SIGNAL_IOError(QString error);
    void SIGNAL_InterruptCommand(QString message);
    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived(); // dbus
    void SIGNAL_Address(int address);
    void SIGNAL_Measurement(double timestamp, double value, bool ol); // dbus
    void SIGNAL_CurrentRange(double value); // dbus
    void SIGNAL_Init();
    void SIGNAL_Mean(double value); // dbus
    void SIGNAL_StdDev(double value); // dbus

    void SIGNAL_ReadAndTimeFormElem();
    void SIGNAL_ConfigCurrent();
    void SIGNAL_ReplyZero();

    void SIGNAL_Zero(double value); // dbus
    void SIGNAL_ZeroRemoved(); // dbus
    void SIGNAL_ZeroForcedRemoved(); // dbus

private:
    void SetupStateMachine();
    void ProcessAddressReply(QByteArray body);
    void PrintStateChanges(QState *state, QString name);
    QString GetReadAddressCommand() const;

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    const QByteArray command_footer_ = "\n";
    global::ThreadSafeQueue<QString> command_queue_;
    int reply_timeout_ = 0;
    int reconnect_interval_ = 1000; // ms
    const size_t MAXQUEUESIZE = 10;
    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;
    double range_ = 20.1e-12;
    double zero_ = 0.0;
    double max_leakage_current_ = 0.0;
    Measurement measurement_;

    medicyc::cyclotron::MessageLoggerInterface logger_;
};

}

#endif
