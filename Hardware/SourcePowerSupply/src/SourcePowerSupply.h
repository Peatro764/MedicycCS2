#ifndef SOURCEPOWERSUPPLY_H
#define SOURCEPOWERSUPPLY_H

#include <vector>
#include <QStateMachine>
#include <QTimer>

#include "PowerSupplyConfig.h"
#include "RegulatedParameter.h"
#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

namespace global = medicyc::cyclotroncontrolsystem::global;

class SourcePowerSupply : public QObject
{
    Q_OBJECT

public:
    explicit SourcePowerSupply(PowerSupplyConfig config);
    ~SourcePowerSupply();

public slots:
    void Configure(double physical_value); // dbus
    void Startup(); // dbus
    void Shutdown(); // dbus
    void SwitchOn(); // dbus
    void SwitchOff(); // dbus
    void Interrupt(); // dbus
    void Increment(int steps); // dbus. steps currenty not implemented
    void Decrement(int steps); // dbus. steps currently not implemented
    void UpdateDesiredValue(double physical_value); // dbus

signals:
    void SIGNAL_DemandForConfiguration(); // dbus
    void SIGNAL_Configuration(double value);
    void SIGNAL_OnTarget(); // dbus
    void SIGNAL_OffTarget(); // dbus
    void SIGNAL_Error(QString message); // dbus
    void SIGNAL_On(); // dbus
    void SIGNAL_Off(); // dbus
    void SIGNAL_Local(); // dbus
    void SIGNAL_Remote(); // dbus

    void SIGNAL_Voltage(double value); // dbus
    void SIGNAL_Current(double value); // dbus
    void SIGNAL_RegulatedParamValue(double value); // dbus

private slots:
    void InterpretOmronBitRead(const QString& channel, bool content);
    void InterpretADCChannelRead(const QString& channel, double value, QString unit);
    void DoRoughStep();
    void DoFineStep();
    void ReadParams();
    void ReadVoltage();
    void ReadCurrent();
    void ReadRegulatedParam();
    void ReadMode();
    void ReadIsOn();
    void WriteOn();
    void WriteOff();
    void IncVoltage();
    void DecVoltage();

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals: // internal
    void SIGNAL_Interrupted(QString message);
    void SIGNAL_StartUp();
    void SIGNAL_ShutDown();
    void SIGNAL_SwitchOn();
    void SIGNAL_SwitchOff();
    void SIGNAL_Increment();
    void SIGNAL_Decrement();
    void SIGNAL_GotoDesValue();
    void SIGNAL_DbusOk();
    void SIGNAL_DbusError();

private:
    void SetupStateMachine();
    void CheckDbus();
    void ConnectSignals();
    void SetupTimers();
    void PrintStateChanges(QAbstractState *state, QString name);
    void CreateStartupStates(global::TimedState *parent);
    void CreateRoughRegulationStates(global::TimedState *parent);
    void CreateFineRegulationStates(global::TimedState *parent);

    struct {
        bool params_set_ = false;
        double physical_value_ = 0.0;
    } startup_parameters_;

    PowerSupplyConfig config_;
    RegulatedParameter voltage_;
    RegulatedParameter regulated_;

    QStateMachine sm_;
    medicyc::cyclotron::ADCInterface adc_;
    medicyc::cyclotron::OmronInterface omron_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
    bool dbus_connected_ = false;
    QTimer tPing_;
};

}

#endif
