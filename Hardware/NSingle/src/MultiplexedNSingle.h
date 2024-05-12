#ifndef MULTIPLEXED_NSINGLE_H
#define MULTIPLEXED_NSINGLE_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <queue>
#include <QSettings>
#include <memory>
#include <QStateMachine>

#include "ThreadSafeQueue.h"
#include "NSingleConfig.h"
#include "MultiplexConfig.h"
#include "NSingleUtil.h"
#include "SocketClient.h"
#include "CommandCreator.h"
#include "ReplyParser.h"

#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

namespace global = medicyc::cyclotroncontrolsystem::global;

class NSINGLE_EXPORT MultiplexedNSingle : public QObject
{
    Q_OBJECT

public:
    MultiplexedNSingle(NSingleConfig nsingle_config, MultiplexConfig multiplex_config);
    ~MultiplexedNSingle();
    NSingleConfig GetNSingleConfig() const { return nsingle_config_; }
    MultiplexConfig GetMultiplexConfig() const { return multiplex_config_; }

public slots:
    void IsConnected();
    void On(); // ANY
    void Off(); // ANY
    void Reset(); // ANY
     void On(QString name);
     void Off(QString name);
     void Reset(QString name);
     void ChangeState(QString name, medicyc::cyclotroncontrolsystem::hardware::nsingle::StateCommandType type);
     void WriteAndVerifyAddressSetPoint(QString name, std::bitset<8> value);
     void WriteAddressSetPoint(QString name, std::bitset<8> value);
     void ReadAddressSetPoint(QString name);
     void ReadAddress(QString name);
     void WriteChannel1SetPoint(QString name, int raw_value, bool sign);
     void WriteChannel1SetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
     void WriteAndVerifyChannel1SetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
     void ReadChannel1SetPoint(QString name);
     void Ping(QString name);
     void ReadChannel1(QString name);
     void ReadChannel1(QString name, int number);
     void WriteChannel1Tolerance(QString name, std::bitset<16> value);
     void Write(QString name, medicyc::cyclotroncontrolsystem::hardware::nsingle::Config config);
     void ReadState(); // ANY
     void ReadConfig(); // ANY
     void ReadError(); // ANY
     void ReadConfig(QString name);
     void ReadState(QString name);
     void ReadError(QString name);

private:
    class MultiplexedCommand {
    public:
        MultiplexedCommand(QString nsingle, std::queue<Command> commands)
            : nsingle_(nsingle), commands_(commands) {}
        MultiplexedCommand(QString nsingle, medicyc::cyclotroncontrolsystem::hardware::nsingle::Command command)
            : nsingle_(nsingle) { commands_.push(command); }
        MultiplexedCommand() {}
        QString nsingle() const { return nsingle_; }
        bool empty() const { return commands_.empty(); }
        size_t size() const { return commands_.size(); }
        Command pop() { Command cmd = commands_.front(); commands_.pop(); return cmd; }
    private:
        QString nsingle_ = "NONE";
        std::queue<Command> commands_;
    };

private slots:     
    void AddToQueue(QString nsingle, medicyc::cyclotroncontrolsystem::hardware::nsingle::Command cmd);
    void AddToQueue(QString nsingle, std::queue<medicyc::cyclotroncontrolsystem::hardware::nsingle::Command> cmds);
    void ProcessQueue();
    void ClearQueue();
    void CheckIOLoad();
    void SelectNSingle();
    void PopCommand();
    void VerifyCurrentNSingle(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address);
    void WarningMessage(QString name, QString message);

signals:
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_Local();
    void SIGNAL_Remote();
    void SIGNAL_Reconnect();
    void SIGNAL_IOError(QString error);
    void SIGNAL_InterruptCommand(QString name, QString message);
    void SIGNAL_IOLoad(double load);

    void SIGNAL_CommandAdded();
    void SIGNAL_ReplyReceived();
    void SIGNAL_ExecutionFinished();
    void SIGNAL_NSingleSuccessfullySelected();
    void SIGNAL_NSingleSelectionMismatch();
    void SIGNAL_MultiplexedCommandFinished();

    void SIGNAL_ConfigUpdated(QString name, medicyc::cyclotroncontrolsystem::hardware::nsingle::Config config);
    void SIGNAL_ErrorUpdated(QString name, medicyc::cyclotroncontrolsystem::hardware::nsingle::Error error);
    void SIGNAL_StateUpdated(QString name, medicyc::cyclotroncontrolsystem::hardware::nsingle::StateReply state_reply);
    void SIGNAL_On(QString name);
    void SIGNAL_Off(QString name);
    void SIGNAL_Error(QString name);
    void SIGNAL_Ok(QString name);
    void SIGNAL_On(); // ANY
    void SIGNAL_Off(); // ANY
    void SIGNAL_Error(); // ANY
    void SIGNAL_Ok(); // ANY
    void SIGNAL_Channel1Value(double physical_value, bool polarity); // ANY
    void SIGNAL_Channel1SetPoint(double physical_value, bool polarity); // ANY

    void SIGNAL_AddressValue(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address);
    void SIGNAL_AddressSetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address);

    void SIGNAL_Channel1Value(QString name, double physical_value, bool polarity); //dbus
    void SIGNAL_Channel1SetPoint(QString name, double physical_value, bool polarity); //dbus
    void SIGNAL_Channel1Value(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void SIGNAL_Channel1SetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);

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

    NSingleConfig nsingle_config_;
    MultiplexConfig multiplex_config_;

    global::ThreadSafeQueue<MultiplexedCommand> cmd_queue_;
    int reply_timeout_ = 0;
    int reconnect_interval_ = 1000;
    MultiplexedCommand active_command_;
    QString active_nsingle_ = "NONE";
    const QString ANY = "ANY";

    const size_t MAXQUEUESIZE = 50;
    QTimer ioload_timer_;
    std::vector<size_t> ioload_;

    medicyc::cyclotron::MessageLoggerInterface logger_;
};

} // namespace

#endif // NSINGLE_H
