#ifndef MIDDLELAYER_CONTROLLER_H
#define MIDDLELAYER_CONTROLLER_H

#include <vector>
#include <QStateMachine>
#include <QTimer>
#include <QMap>

#include "Niveau.h"
#include "Dee.h"
#include "Membrane.h"
#include "HFUtils.h"
#include "VoltageRegulator.h"
#include "Accord.h"
#include "TimedState.h"
#include "HFRepo.h"
#include "HFState.h"

#include "signalgenerator_interface.h"
#include "hfpowersupplies_interface.h"
#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

class HFController : public QObject
{
    Q_OBJECT

public:
    HFController();
    ~HFController();

public slots:
    void Startup();
    void Shutdown();
    void Interrupt();
    void IncNiveau(int hf);
    void DecNiveau(int hf);
    void IncFrequency();
    void DecFrequency();
    void SetFrequency(double value);
    void InitGenerator();
    void FindAccord() { accords_[HFX::HF1]->FindAccord(); } // TODO remove after testing

    void Configure_VoltageDeltaUp(double value);
    void Configure_VoltageDeltaDown(double value);
    void Configure_VoltagePostAccord(HFX hf, double value);

signals:
    // Public
    void SIGNAL_State_Off();
    void SIGNAL_State_Ready();
    void SIGNAL_State_Intermediate();
    void SIGNAL_State_Unknown();
    void SIGNAL_StartupFinished();
    void SIGNAL_ShutdownFinished();
    void SIGNAL_Niveau_Value(int hf, double value);
    void SIGNAL_Niveau_State(int hf, bool on);
    void SIGNAL_Dee_Voltage(int hf, double value);
    void SIGNAL_Frequency(double value);

    // Internal
    void SIGNAL_Startup();
    void SIGNAL_Shutdown();
    void SIGNAL_Interrupt();
    void SIGNAL_Error();

private slots:    
    void GetConfiguration(int id);
    void GetLastConfiguration();
    void UpdateConfiguration(Configuration config);
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void SetupComponents();
    void ReadConfiguration();
    void ConnectSignals();
    void SetupStateMonitoring();
    void SetupStateMachine();
    QState* CreateSetCleanInitialState(QState *parent, QState *error);
    QState* CreateStartupPowerSuppliesState(QState *parent, QState *error);
    QState* CreateShutdownPowerSuppliesState(QState *parent, QState *error);
    QState* CreateSwitchOnNiveauxState(QState *parent, QState *error);
    QState* CreateAccordMembranesState(QState *parent, QState *error);
    QState* CreateDownFrequencyState(QState *parent, QState *error);
    QState* CreateActivateRegulationState(QState *parent, QState *error);
    QState* CreateDeactivateRegulationState(QState *parent, QState *error);
    QState* CreateSetShutdownInitialNiveau(QState *parent, QState *error);
    QState* CreateReduceVoltageDee(QState *parent, QState *error);
    QState* CreateUpFrequencyState(QState *parent, QState *error);
    QState* CreateManuelMembranesState(QState *parent, QState *error);
    QState* CreateSwitchOffNiveauxState(QState *parent, QState *error);
    QState* CreatePrepositionMembranesState(QState *parent, QState *error);
    void PrintStateChanges(QAbstractState *state, QString name);

    QStateMachine sm_;
    HFState state_;

    QMap<HFX, Niveau*> niveaux_;
    QMap<HFX, Membrane*> membranes_;
    QMap<HFX, Dee*> dees_;
    QMap<HFX, Accord*> accords_;
    QMap<HFX, VoltageRegulator*> v_regulators_;

    HFRepo repo_;
    medicyc::cyclotron::SignalGeneratorInterface generator_;
    medicyc::cyclotron::HFPowerSuppliesInterface powersupplies_;
    medicyc::cyclotron::MessageLoggerInterface logger_;

    double voltage_dUp_ = 5.0; // delta increment voltage during down frequency procedure
    double voltage_dDown_ = 5.0; // delta decrement voltage during up frequency procedure
    QMap<HFX, double> voltage_post_accord_;

};

}

#endif
