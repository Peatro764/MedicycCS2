#ifndef MIDDLELAYER_HF_ACCORD_H
#define MIDDLELAYER_HF_ACCORD_H

#include <QStateMachine>
#include <QTimer>
#include <QObject>

#include <Membrane.h>
#include <Dee.h>
#include <Niveau.h>
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

class Accord : public QObject
{
    Q_OBJECT
public:
    Accord(HFX hf, Membrane* membrane, Dee* dee, Niveau* niveau);
    void FindAccord();
    void Interrupt();
    void Ping();

    void Configure_VoltageLower(double value);
    void Configure_VoltageUpper(double value);
    void Configure_PhaseLower(double value);
    void Configure_PhaseUpper(double value);

signals:
    // public
    void SIGNAL_Accorded();
    void SIGNAL_NotAccorded();
    void SIGNAL_VoltageBelowLowerLimit();
    void SIGNAL_VoltageAboveLowerLimit();
    void SIGNAL_VoltageAboveUpperLimit();
    void SIGNAL_VoltageBelowUpperLimit();

    void SIGNAL_Interrupt();
    void SIGNAL_Error();

    // intern
    void SIGNAL_FindAccord();

private slots:
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);
    void CheckAccord();
    void CheckVoltageLimits();

private:
    void ConnectSignals();
    void SetupStateMachine();
    global::TimedState* GetScanState(QState *parent, int timeout_ms, QString timeout_message, QState *timeout_state);
    void PrintStateChanges(QAbstractState *state, QString name);
    QString GetHFBranch();
    HFX hfx_;
    Membrane* membrane_;
    Dee* dee_;
    Niveau* niveau_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    QStateMachine sm_;
    double voltage_ = 0.0;
    double phase_ = 0.0;
    double voltage_lower_ = 8.0;
    double voltage_upper_ = 15.0; // if V increases above, sm start to decrease niveau
    double phase_lower_ = 2.5;
    double phase_upper_ = 7.5;

};

}

#endif // ACCORD_H
