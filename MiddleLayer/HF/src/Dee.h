#ifndef MIDDLELAYER_HF_DEE_H
#define MIDDLELAYER_HF_DEE_H

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
 *  Implements the functionality to read Dee voltage and phase,
 *  and turn on/off the regulation of the former.
 *  Reads these parameters automatically at 0.5 Hz
 */

class Dee : public QObject
{
    Q_OBJECT

public:
    explicit Dee(HFX hfx);
    ~Dee();

public slots:
    void SetRegulationOn();
    void SetRegulationOff();
    void IsRegulationOn();
    void ReadDeeVoltage();
    void ReadDeePhase();
    void Ping();

signals:
    // Public
    void SIGNAL_RegulationOn();
    void SIGNAL_RegulationOff();
    void SIGNAL_DeeVoltage(double kV);
    void SIGNAL_DeePhase(double value);
    void SIGNAL_VoltageIncreasing();
    void SIGNAL_VoltageDecreasing();

    // Private
    void SIGNAL_WriteRegulationOn_True();
    void SIGNAL_WriteRegulationOn_False();
    void SIGNAL_WriteRegulationOff_True();
    void SIGNAL_WriteRegulationOff_False();

    void SIGNAL_CmdRegulationOn();
    void SIGNAL_CmdRegulationOff();

    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_Error();

private slots:
    void InterpretOmronBitRead(const QString& channel, bool content);
    void InterpretADCChannelRead(const QString& channel, double value);
    void CheckVoltage(double value);
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void ConnectSignals();
    void SetupStateMachine();
    void PrintStateChanges(QAbstractState *state, QString name);
    QString GetHFBranch() const;

    QString CmdStringWriteRegulationOn() { return "C " + GetHFBranch() + " On Regulation"; }
    QString CmdStringWriteRegulationOff() { return "C " + GetHFBranch() + " Off Regulation"; }
    QString CmdStringIsRegulationOn() { return "Q " + GetHFBranch() + " Regulation On"; }
    QString CmdStringReadDeeVoltage() { return "V." + GetHFBranch(); }
    QString CmdStringReadDeePhase() { return "Phase " + GetHFBranch(); }

    HFX hfx_;
    QStateMachine sm_;
    medicyc::cyclotron::ADCInterface adc_;
    medicyc::cyclotron::OmronInterface omron_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    const int PULSE_WIDTH = 50;
    double phase_ = 0.0;
    double voltage_ = 0.0;
    int voltage_direction_ = 0;
    const int DIRECTION_COUNTS = 3;
    const int ping_interval_ = 2000;
};

}

#endif
