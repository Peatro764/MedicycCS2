#ifndef MIDDLELAYER_HF_VOLTAGEREGULATOR_H
#define MIDDLELAYER_HF_VOLTAGEREGULATOR_H

#include <QStateMachine>
#include <QTimer>
#include <QObject>
#include <Dee.h>
#include <Niveau.h>
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

/*
 * The class implements basic functionality to reach a setpoint
 * value of the HF Dee voltage. It increments or decrements the Niveau
 * in steps to reach the Dee Voltage setpoint given by the user of the class.
 * A maximum allowed Dee voltage can be configured.
 */

class VoltageRegulator : public QObject
{
    Q_OBJECT
public:
    VoltageRegulator(HFX hf, Dee* dee, Niveau* niveau);
    void SetVoltage(double kV);
    void IncrementVoltage(double dkV);
    void DecrementVoltage(double dkV);
    void Interrupt();

    void Configure_VoltageMax(double value);

signals:
    void SIGNAL_SetPointUpdated();
    void SIGNAL_SetPointAboveMax();
    void SIGNAL_ActAboveSet();
    void SIGNAL_ActBelowSet();
    void SIGNAL_ActAboveMax();
    void SIGNAL_Error();
    void SIGNAL_VoltageSet();
    void SIGNAL_Interrupt();

private slots:
    void UpdateDeeVoltage(double kv);
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void ConnectSignals();
    void SetupStateMachine();
    void PrintStateChanges(QAbstractState *state, QString name);
    QString GetHFBranch();
    HFX hfx_;
    Dee* dee_;
    Niveau* niveau_;
    double voltage_actual_ = 0.0;
    double voltage_setpoint_ = 0.0;
    double voltage_max_ = 45.0;
    double MIN_VOLTAGE_ = 5.0; // cannot regulate a voltage below this value since niveau=0 still gives a voltage
    medicyc::cyclotron::MessageLoggerInterface logger_;
    QStateMachine sm_;
};

}

#endif // ACCORD_H
