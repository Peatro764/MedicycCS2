#ifndef MIDDLELAYER_HF_NIVEAU_H
#define MIDDLELAYER_HF_NIVEAU_H

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
 * Implements the functionality to switch on/off, inc/dec and set the HF "Niveau",
 * which corresponds to the amplification of the HF signal between the generator
 * and the preampls. Commands are sent to the OMRON Cyclo PLC, which executes them.
 * The actual Niveau value is read from the ADC.
 * Inc/dec commands are active during a set time (250 ms), and thus sends a short pulse
 * to the OMRON PLC during which the Niveau is changed.
 * SetLevel commands set the command high until the desired level is reached as read back
 * by the ADC.
 *
 */

class Niveau : public QObject
{
    Q_OBJECT

public:
    explicit Niveau(HFX hfx);
    ~Niveau();

public slots:
    void On();
    void Off();
    void Increment();
    void Decrement();
    void SetStartupFinalLevel();
    void SetShutdownInitialLevel();
    void Ping();

    void SetLevel(double value); // TODO remove
    void Interrupt();

    void Configure_NiveauBeforeOn(double value);
    void Configure_NiveauBeforeOff(double value);
    void Configure_NiveauStartupFinalValue(double value);
    void Configure_NiveauShutdownInitialValue(double value);
    void Configure_NiveauMax(double value);

signals:
    // Public //
    void SIGNAL_On();
    void SIGNAL_Off();
    void SIGNAL_Value(double div);
    void SIGNAL_Interrupt();

    void SIGNAL_AboveShutdownInitialValue();
    void SIGNAL_BelowShutdownInitialValue();

    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_Error();

    //  Private //
    void SIGNAL_SetStartupFinalValue();
    void SIGNAL_SetShutdownInitialValue();
    void SIGNAL_ValueReached();
    void SIGNAL_ActBelowDes();
    void SIGNAL_ActAboveDes();
    void SIGNAL_ActBelowMax();
    void SIGNAL_ActAboveMax();


    void SIGNAL_CmdOn();
    void SIGNAL_CmdOff();
    void SIGNAL_CmdIncrement();
    void SIGNAL_CmdDecrement();
    void SIGNAL_CmdSetLevel(); // TODO REmove

    // Replies to omron commands
    void SIGNAL_OnAllowed_True();
    void SIGNAL_OnAllowed_False();
    void SIGNAL_Increment_True();
    void SIGNAL_Increment_False();
    void SIGNAL_Decrement_True();
    void SIGNAL_Decrement_False();
    void SIGNAL_WriteOn_True();
    void SIGNAL_WriteOn_False();
    void SIGNAL_WriteOff_True();
    void SIGNAL_WriteOff_False();

private slots:
    void IsAllowedOn();
    void IsOn();
    void ReadValue();
    void CheckValueThresholds(double value);
    void InterpretOmronBitRead(const QString& channel, bool content);
    void InterpretADCChannelRead(const QString& channel, double value);

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void ConnectSignals();
    void SetupStateMachine();
    void CreateOnState(QState *parent);
    void CreateOffState(QState *parent);
    void CreateSetLevelState(QState *parent, QState *error);
    void CreateIncState(QState *parent);
    void CreateDecState(QState *parent);

    void PrintStateChanges(QAbstractState *state, QString name);
    QString GetHFBranch() const;

    QString CmdStringIncrement() { return "C " + GetHFBranch() + " Up Niveau"; }
    QString CmdStringDecrement() { return "C " + GetHFBranch() + " Down Niveau"; }
    QString CmdStringIsAllowedOn() { return "Q " + GetHFBranch() + " Autorisation Niveau"; }
    QString CmdStringIsOn() { return "Q " + GetHFBranch() + " Niveau On"; }
    QString CmdStringReadValue() { return "Niveau " + GetHFBranch(); }
    QString CmdStringWriteOn() { return "C " + GetHFBranch() + " Niveau On"; }
    QString CmdStringWriteOff() { return "C " + GetHFBranch() + " Niveau Off"; }

    HFX hfx_;
    double level_set_ = 0; // TODO Remove after test
    double level_act_ = 0; // last read value
    double level_before_on_ = 1.4; // This level is set before switching on Niveau
    double level_before_off_ = 1.0; // This level is set before switching off Niveau
    double level_startup_final_ = 4.5; // This level is set after switching on regul in the startup phase
    double level_shutdown_initial_ = 3.0; // This level is set at the start of the shutdown
    double level_max_ = 6.0; // This is the max allowed level
    double level_desired_ = 0.0; // Last commanded setpoint value
    QStateMachine sm_;
    medicyc::cyclotron::ADCInterface adc_;
    medicyc::cyclotron::OmronInterface omron_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    const int PULSE_WIDTH = 50; // For pulsed omron commands, this is the pulse length
    int incdec_pulse_length_ = 250; // When doing an inc or dec niveau, this is the length the cmd is active
    const int ping_interval_ = 2000;
};

}

#endif
