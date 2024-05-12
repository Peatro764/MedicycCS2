#ifndef MIDDLELAYER_HF_MEMBRANE_H
#define MIDDLELAYER_HF_MEMBRANE_H

#include <vector>
#include <QStateMachine>
#include <QTimer>

#include "HFUtils.h"
#include "TimedState.h"
#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

/*
 * Implements the functionality to move and monitor the membranes.
 *
 * Reads membrane parameters (position, mode, limitswitch) at
 * 1 or 0.2 Hz (user defined).
 *
 *
 */

class Membrane : public QObject
{
    Q_OBJECT

public:
    explicit Membrane(HFX hf);
    ~Membrane();

    void SetModeAutomatic();
    void SetModeManual();
    void MoveToAccordPosition();
    void StartMovingUp();
    void StartMovingDown();
    void Interrupt();
    void Stop();
    void Ping();
    void SaveAccordPosition();

    void Configure_UpperLimit(double value);
    void Configure_LowerLimit(double value);
    void Configure_LimitClearance(double value);

private slots:
    void ReadIsMovingUp();
    void ReadIsMovingDown();
    void ReadPosition();
    void ReadMode();
    void ReadLimitSwitch();
    void SetFastPing();
    void SetSlowPing();

signals:
    void SIGNAL_Error();
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();

    void SIGNAL_Automatic();
    void SIGNAL_Manual();
    void SIGNAL_Stopped();
    void SIGNAL_MovingUp();
    void SIGNAL_NotMovingUp();
    void SIGNAL_MovingDown();
    void SIGNAL_NotMovingDown();
    void SIGNAL_HWLimitReached();
    void SIGNAL_HWLimitNotReached();
    void SIGNAL_SWUpperLimitReached();
    void SIGNAL_SWUpperLimitNotReached();
    void SIGNAL_SWLowerLimitReached();
    void SIGNAL_SWLowerLimitNotReached();
    void SIGNAL_ClearedFromLimits();
    void SIGNAL_AccordPositionReached();
    void SIGNAL_Position(double percent);

    // High-level process commands
    void SIGNAL_SetModeAutomatic();
    void SIGNAL_SetModeManual();
    void SIGNAL_StartMovingUp();
    void SIGNAL_StartMovingDown();
    void SIGNAL_Stop();

    // Omron commands
    void SIGNAL_WriteModeAuto_True();
    void SIGNAL_WriteModeAuto_False();
    void SIGNAL_WriteModeManu_True();
    void SIGNAL_WriteModeManu_False();

private slots:
    void InterpretOmronBitRead(const QString& channel, bool content);
    void InterpretADCChannelRead(const QString& channel, double value);
    void PositionReceived(double value);

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void ConnectSignals();
    void SetupStateMachine();
    QState* GetModeAutoState(QState *parent);
    QState* GetModeManualState(QState *parent);
    void PrintStateChanges(QAbstractState *state, QString name);
    QString GetHFBranch() const;
    // Command strings
    QString CmdStringSetModeAutomatic() { return "C " + GetHFBranch() + " Auto Membrane"; }
    QString CmdStringSetModeManual() { return "C " + GetHFBranch() + " Manu Membrane"; }
    QString CmdStringMoveUp() { return "C " + GetHFBranch() + " + Membrane"; }
    QString CmdStringMoveDown() { return "C " + GetHFBranch() + " - Membrane"; }
    QString CmdStringReadPosition() { return "Membrane " + GetHFBranch(); }
    QString CmdStringReadMode() { return "Q " + GetHFBranch() + " Membrane Auto"; }
    QString CmdStringReadLimitSwitches() { return "Q Fin de course membranes"; }

    HFX hfx_;
    double accord_position_ = 50.0;
    double current_position_ = 50.0;
    double upper_limit_ = 85.0;
    double lower_limit_ = 15.0;
    double limit_clearance_ = 5.0; // distance from software limit where membrane is in intermediare position
    QStateMachine sm_;
    medicyc::cyclotron::ADCInterface adc_;
    medicyc::cyclotron::OmronInterface omron_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    const int PULSE_WIDTH = 50;
    QTimer ping_timer_;
    const int ping_fast_interval_ = 1000;
    const int ping_slow_interval_ = 5000;
};

}

#endif
