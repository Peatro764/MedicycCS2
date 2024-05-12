#ifndef NSINGLE_H
#define NSINGLE_H

#include "INSingle.h"
#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <queue>
#include <QSettings>
#include <memory>
#include <QStateMachine>

#include "ThreadSafeQueue.h"
#include "NSingleUtil.h"
#include "SocketClient.h"
#include "CommandCreator.h"
#include "ReplyParser.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

namespace global = medicyc::cyclotroncontrolsystem::global;

class NSINGLE_EXPORT NSingle : public QObject, public INSingle
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::nsingle::INSingle)
    // TODO update interface name once namespaces has changed
    Q_CLASSINFO("D-Bus Interface", "medicyc.cyclotron.NSingleInterface")

public:
    NSingle(NSingleConfig config);
    ~NSingle();
    NSingleConfig config() const override { return config_; }
    QString name() const override { return config_.name(); }

public slots:
    void IsConnected() override; // DBus
     void On() override { ChangeState(StateCommandType::On); } // DBus
     void Off() override { ChangeState(StateCommandType::Off); } // DBus
     void Reset() override; // dbus
     void ChangeState(medicyc::cyclotroncontrolsystem::hardware::nsingle::StateCommandType type);
     void WriteAndVerifyAddressSetPoint(std::bitset<8> value) override;
     void WriteAddressSetPoint(std::bitset<8> value) override;
     void ReadAddressSetPoint() override;
     void ReadAddress() override;
     void Ping() override;
     void WriteAndVerifyChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) override;
     void WriteChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) override;
     void WriteChannel1SetPoint(int raw_value, bool sign);
     void WriteChannel1SetPoint(std::bitset<16> data);
     void WriteChannel1SetPoint(double interpreted_value, bool sign);
     void ReadChannel1SetPoint() override;
     void ReadChannel1() override;
     void ReadChannel1(int number) override;
     void WriteChannel1Tolerance(std::bitset<16> value);
     void ReadChannel2();
     void Write(medicyc::cyclotroncontrolsystem::hardware::nsingle::Config config);
     void ReadConfig(); // Register Z (reads all config parameters)
     void ReadState() override;  // Register C
     void ReadError(); // Register X

private slots:
    void AddToQueue(medicyc::cyclotroncontrolsystem::hardware::nsingle::Command command);
    void AddToQueue(std::queue<Command> commands);
    void ProcessQueue();
    void ClearQueue();
    void CheckIOLoad();
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);

signals:
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_On() override; // dbus
    void SIGNAL_Off() override; // dbus
    void SIGNAL_Error() override; // dbus
    void SIGNAL_Ok() override; // dbus
    void SIGNAL_Local() override; // dbus
    void SIGNAL_Remote() override; // dbus
    void SIGNAL_IOLoad(double load) override; // dbus

    void SIGNAL_Reconnect();
    void SIGNAL_InterruptCommand(QString message);

    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived() override;
    void SIGNAL_ExecutionFinished() override;

    void SIGNAL_ConfigUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::Config config);
    void SIGNAL_ErrorUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::Error error);
    void SIGNAL_StateUpdated(medicyc::cyclotroncontrolsystem::hardware::nsingle::StateReply state_reply);

    void SIGNAL_AddressValue(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) override;
    void SIGNAL_AddressSetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) override;

    void SIGNAL_Channel1Value(double physical_value, bool polarity) override; //dbus
    void SIGNAL_Channel1SetPoint(double physical_value, bool polarity) override; //dbus
    void SIGNAL_Channel1Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) override;
    void SIGNAL_Channel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) override;
    void SIGNAL_Channel2Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);

private:
    void ConnectReplyParserSignals();
    void ConnectSocketSignals();
    void SetupStateMachine();
    void PrintStateChanges(QState *state, QString name);

    std::unique_ptr<QSettings> settings_;
    CommandCreator command_creator_;
    ReplyParser reply_parser_;
    QStateMachine sm_;
    SocketClient socket_client_;

    NSingleConfig config_;

    global::ThreadSafeQueue<Command> cmd_queue_;
    int reply_timeout_ = 0;
    int ping_interval_ = 1000; // ms
    int reconnect_interval_ = 1000;
    const size_t MAXQUEUESIZE = 50;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    double io_load_ = 0.0;
    QTimer ioload_calc_timer_;
    QTimer ioload_emit_timer_;
};

} // namespace

#endif // NSINGLE_H
