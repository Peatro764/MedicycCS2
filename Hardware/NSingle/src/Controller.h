#ifndef NSINGLE_CONTROLLER_H
#define NSINGLE_CONTROLLER_H

#include "NSingle_global.h"
#include "INSingle.h"

#include <QObject>
#include <QString>
#include <QSettings>
#include <QStateMachine>
#include <QTimer>
#include <memory>

#include "TimedState.h"
#include "RegulatedParameter.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

namespace global = medicyc::cyclotroncontrolsystem::global;

class NSINGLE_EXPORT Controller : public QObject
{
    Q_OBJECT
    // TODO update interface name once namespaces has changed
    Q_CLASSINFO("D-Bus Interface", "medicyc.cyclotron.NSingleControllerInterface")

public:
    Controller(INSingle *nsingle, int start_delay);
    ~Controller();

public slots:
    void Configure(double physical_value, bool polarity);
    void QueryState();
    void Startup();
    void Shutdown();
    void Interrupt();
    void Increment(int nbits);
    void Decrement(int nbits);
    void UpdateDesiredValue(double physical_value);
    void ChangePolarity();
    void SetActiveCompensation(bool on);

private slots:
    void Reset();
    void On();
    void Off();
    void ReadState();
    void ReadChannel1();
    void Ping();
    void ReadMultiChannel1(int number);
    void ReadChannel1SetPoint();   
    void WriteChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void WriteAndVerifyChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void DecideRegulationType();
    void EmitSignalsToUI();
    void LocalMode();
    void RemoteMode();

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals:
    // external interface
    void SIGNAL_Configuration(double physical_value, bool polarity);
    void SIGNAL_DemandForConfiguration();
    void SIGNAL_OnTarget();
    void SIGNAL_OffTarget();
    void SIGNAL_Error(QString message);
    void SIGNAL_On();
    void SIGNAL_Off();
    void SIGNAL_TransitionToOff();
    void SIGNAL_TransitionToOn();
    void SIGNAL_Done();
    void SIGNAL_ActValue(double physical_value, bool polarity);
    void SIGNAL_DesValue(double physical_value, bool polarity);
    void SIGNAL_ActiveCompensation(bool on);
    void SIGNAL_HardwareError(); // Defaut
    void SIGNAL_HardwareOk();
    void SIGNAL_Disconnected();
    void SIGNAL_Connected();
    void SIGNAL_IOLoad(double load);

    // internal
    void SIGNAL_Startup();
    void SIGNAL_Shutdown();
    void SIGNAL_Interrupt(QString message);
    void SIGNAL_FineAdjust();
    void SIGNAL_RoughAdjust();
    void SIGNAL_StepAdjust();
    void SIGNAL_ChangePolarity();
    void SIGNAL_DesValueNotAllowed();

private:
    void ConnectSignals();
    void SetupTimers();

    void SetupStateMachine();
    void CreateCheckActValueStates(global::TimedState *parent, QState *sOnTarget, QState *sOffTarget);
    void CreateCheckStateStates(global::TimedState *parent, QState *sOn, QState *sOff);
    void CreateStartupStates(global::TimedState *parent);
    void CreateRoughRegulationStates(global::TimedState *parent);
    void CreateFineRegulationStates(global::TimedState *parent);
    void CreateStepAdjustStates(global::TimedState *parent);
    void CreateReadyStates(QState *parent);
    void CreateShutdownStates(global::TimedState *parent);
    void CreateChangePolarityStates(global::TimedState *parent);
    void PrintStateChanges(QAbstractState *state, QString name);
    bool InRange(double physical_value) const;

    struct {
        bool params_set_ = false;
        double physical_value_ = 0.0;
        bool polarity_ = true;
    } startup_parameters_;

    QStateMachine sm_;
    INSingle* nsingle_;
    RegulatedParameter regulated_;
    bool active_compensation_ = false;
    bool local_mode_ = false;

    QTimer tPing_; // read state and channel1 value

    medicyc::cyclotron::MessageLoggerInterface logger_;
};

} // namespace

#endif
