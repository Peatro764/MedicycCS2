#include "HFController.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

#include "DBus.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

HFController::HFController() :
    repo_("_prod"),
    generator_("medicyc.cyclotron.hardware.signalgenerator", "/SignalGenerator", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    powersupplies_("medicyc.cyclotron.hardware.hfpowersupplies", "/PowerSupplies", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    Configure_VoltagePostAccord(HFX::HF1, 17);
    Configure_VoltagePostAccord(HFX::HF2, 17);

    SetupComponents();
    GetLastConfiguration();
    SetupStateMonitoring();
    ConnectSignals();
    SetupStateMachine();

    qDebug() << "Controller\n"
             << "Generator DBUS " << generator_.isValid() << "\n"
             << "PowerSupplies DBUS " << powersupplies_.isValid();
}

HFController::~HFController() {
    for (auto& n : accords_.keys()) {
        delete accords_[n];
    }
    accords_.clear();

    for (auto& n : niveaux_.keys()) {
        delete niveaux_[n];
    }
    niveaux_.clear();

    for (auto& n : membranes_.keys()) {
        delete membranes_[n];
    }
    membranes_.clear();

    for (auto& n : dees_.keys()) {
        delete dees_[n];
    }
    dees_.clear();

    for (auto& n : v_regulators_.keys()) {
        delete v_regulators_[n];
    }
    v_regulators_.clear();
}

void HFController::Configure_VoltageDeltaUp(double value) {
    voltage_dUp_ = value;
    qDebug() << "HFController::Configure_VoltageDeltaUp " << value;
}

void HFController::Configure_VoltageDeltaDown(double value) {
    voltage_dDown_ = value;
    qDebug() << "HFController::Configure_VoltageDeltaDown " << value;
}

void HFController::Configure_VoltagePostAccord(HFX hf, double value) {
    voltage_post_accord_[hf] = value;
    qDebug() << "HFController::Configure_VoltageHF1PostAccord " << static_cast<int>(hf) << " " << value;
}

void HFController::Interrupt() {
    qDebug() << "HFController::Interrupt";
    membranes_[HFX::HF1]->Interrupt();
    membranes_[HFX::HF2]->Interrupt();
    niveaux_[HFX::HF1]->Interrupt();
    niveaux_[HFX::HF2]->Interrupt();
    accords_[HFX::HF1]->Interrupt();
    accords_[HFX::HF2]->Interrupt();
    v_regulators_[HFX::HF1]->Interrupt();
    v_regulators_[HFX::HF2]->Interrupt();
    // Dee does not have an interrupt
    emit SIGNAL_Interrupt();
}

void HFController::SetupComponents() {
    niveaux_[HFX::HF1] = new Niveau(HFX::HF1);
    niveaux_[HFX::HF2] = new Niveau(HFX::HF2);

    membranes_[HFX::HF1] = new Membrane(HFX::HF1);
    membranes_[HFX::HF2] = new Membrane(HFX::HF2);

    dees_[HFX::HF1] = new Dee(HFX::HF1);
    dees_[HFX::HF2] = new Dee(HFX::HF2);

    accords_[HFX::HF1] = new Accord(HFX::HF1, membranes_[HFX::HF1], dees_[HFX::HF1], niveaux_[HFX::HF1]);
    accords_[HFX::HF2] = new Accord(HFX::HF2, membranes_[HFX::HF2], dees_[HFX::HF2], niveaux_[HFX::HF2]);

    v_regulators_[HFX::HF1] = new VoltageRegulator(HFX::HF1, dees_[HFX::HF1], niveaux_[HFX::HF1]);
    v_regulators_[HFX::HF2] = new VoltageRegulator(HFX::HF2, dees_[HFX::HF2], niveaux_[HFX::HF2]);
}

void HFController::GetLastConfiguration() {
    try {
        auto conf_hf1 = repo_.GetLastConfiguration(HFX::HF1);
        auto conf_hf2 = repo_.GetLastConfiguration(HFX::HF2);
        UpdateConfiguration(conf_hf1);
        UpdateConfiguration(conf_hf2);
    } catch (std::exception& exc) {
        qWarning() << "HFController::GetLastConfiguration Caught exception " << exc.what();
        ErrorMessage("Echec de la récupération de la configuration");
    }
}

void HFController::GetConfiguration(int id) {
    try {
        auto config = repo_.GetConfiguration(id);
        UpdateConfiguration(config);
    } catch (std::exception& exc) {
        qWarning() << "HFController::GetConfiguration Caught exception " << exc.what();
        ErrorMessage("Echec de la récupération de la configuration");
    }
}

void HFController::UpdateConfiguration(Configuration config) {
    qDebug() << "HFController::UpdateConfiguration HF" << QString::number(static_cast<int>(config.hf())) << " " << config.timestamp();
    InfoMessage("Mise à jour de la configuration HF" + QString::number(static_cast<int>(config.hf())));
    qDebug() << "HFController::UpdateConfiguration Voltage post accord " << config.voltage_post_accord();
    HFX hf = config.hf();
    membranes_[hf]->Configure_LowerLimit(config.membrane_min());
    membranes_[hf]->Configure_UpperLimit(config.membrane_max());
    membranes_[hf]->Configure_LimitClearance(config.membrane_clearance());
    niveaux_[hf]->Configure_NiveauBeforeOn(config.niveau_preon_value());
    niveaux_[hf]->Configure_NiveauBeforeOff(config.niveau_preoff_value());
    niveaux_[hf]->Configure_NiveauStartupFinalValue(config.niveau_startup_final_value());
    niveaux_[hf]->Configure_NiveauShutdownInitialValue(config.niveau_shutdown_initial_value());
    niveaux_[hf]->Configure_NiveauMax(config.niveau_max());
    accords_[hf]->Configure_VoltageLower(config.accord_voltage_lower());
    accords_[hf]->Configure_VoltageUpper(config.accord_voltage_upper());
    accords_[hf]->Configure_PhaseLower(config.accord_phase_lower());
    accords_[hf]->Configure_PhaseUpper(config.accord_phase_upper());
    v_regulators_[hf]->Configure_VoltageMax(config.voltage_max());
    Configure_VoltageDeltaDown(config.voltage_delta_down());
    Configure_VoltageDeltaUp(config.voltage_delta_up());
    Configure_VoltagePostAccord(hf, config.voltage_post_accord());
}

void HFController::SetupStateMonitoring() {
    // Frequency generator
    QObject::connect(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyWarm, this, [&]() { state_.SetFrequencyWarm(true); });
    QObject::connect(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyAboveWarm, this, [&]() { state_.SetFrequencyWarm(false); });
    QObject::connect(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyCold, this, [&]() { state_.SetFrequencyCold(true); });
    QObject::connect(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyBelowCold, this, [&]() { state_.SetFrequencyCold(false); });

    // Niveau
    for (auto& n : niveaux_.keys()) {
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_On, this, [&, n]() {
            state_.SetNiveauOn(n, true);
        });
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_Off, this, [&, n]() {
            state_.SetNiveauOn(n, false);
        });
    }

    // Dees
    for (auto& n : dees_.keys()) {
        QObject::connect(dees_[n], &Dee::SIGNAL_DeeVoltage, this, [&, n](double value) {
            state_.SetDeeVoltage(n, value);
        });
        QObject::connect(dees_[n], &Dee::SIGNAL_RegulationOn, this, [&, n]() {
            state_.SetRegulation(n, true);
        });
        QObject::connect(dees_[n], &Dee::SIGNAL_RegulationOff, this, [&, n]() {
            state_.SetRegulation(n, false);
        });
    }

    // Power supplies
    QObject::connect(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_True, this, [&]() { state_.SetPowerSuppliesAllOn(true); });
    QObject::connect(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_False, this, [&]() { state_.SetPowerSuppliesAllOn(false); });
    QObject::connect(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOff_True, this, [&]() { state_.SetPowerSuppliesAllOff(true); });
    QObject::connect(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOff_False, this, [&]() { state_.SetPowerSuppliesAllOff(false); });

    // Pass on the resulting state to above layers
    QObject::connect(&state_, &HFState::SIGNAL_Unknown, this, &HFController::SIGNAL_State_Unknown);
    QObject::connect(&state_, &HFState::SIGNAL_On, this, &HFController::SIGNAL_State_Ready);
    QObject::connect(&state_, &HFState::SIGNAL_Off, this, &HFController::SIGNAL_State_Off);
    QObject::connect(&state_, &HFState::SIGNAL_Intermediate, this, &HFController::SIGNAL_State_Intermediate);
}

void HFController::ConnectSignals() {
    // Frequency generator
    QObject::connect(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_Frequency, this, &HFController::SIGNAL_Frequency);

    // Niveau
    for (auto& n : niveaux_.keys()) {
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_Value, this, [&, n](double value) { emit SIGNAL_Niveau_Value(static_cast<int>(n), value); });
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_On, this, [&, n]() {
            emit SIGNAL_Niveau_State(static_cast<int>(n), true);
        });
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_Off, this, [&, n]() {
            emit SIGNAL_Niveau_State(static_cast<int>(n), false);
        });
        QObject::connect(niveaux_[n], &Niveau::SIGNAL_Error, this, &HFController::SIGNAL_Error);
    }

    // Dees
    for (auto& n : dees_.keys()) {
        QObject::connect(dees_[n], &Dee::SIGNAL_DeeVoltage, this, [&, n](double value) {
            emit SIGNAL_Dee_Voltage(static_cast<int>(n), value);
        });
        QObject::connect(dees_[n], &Dee::SIGNAL_Error, this, &HFController::SIGNAL_Error);
    }

    // Accords, membranes and voltage regulators
    for (auto& n : accords_.keys()) {
        QObject::connect(accords_[n], &Accord::SIGNAL_Error, this, &HFController::SIGNAL_Error);
    }    
    for (auto& n : membranes_.keys()) {
        QObject::connect(membranes_[n], &Membrane::SIGNAL_Error, this, &HFController::SIGNAL_Error);
    }
    for (auto& n : v_regulators_.keys()) {
        QObject::connect(v_regulators_[n], &VoltageRegulator::SIGNAL_Error, this, &HFController::SIGNAL_Error);
    }

    // Power supplies
    QObject::connect(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Error, this, &HFController::SIGNAL_Error);

    QObject::connect(&repo_, &HFRepo::SIGNAL_NewConfigurationAdded, this, &HFController::GetConfiguration);
}

void HFController::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        QState *sError = new QState(sSuperState);
        QState *sStartupSequence = new QState(sSuperState);
            QState *sSetCleanInitialState = CreateSetCleanInitialState(sStartupSequence, sError);
            QState *sStartupPowerSupplies = CreateStartupPowerSuppliesState(sStartupSequence, sError);
            QState *sSwitchOnNiveaux = CreateSwitchOnNiveauxState(sStartupSequence, sError);
            QState *sAccordMembranes = CreateAccordMembranesState(sStartupSequence, sError);
            QState *sDownFrequency = CreateDownFrequencyState(sStartupSequence, sError);
            QState *sActivateRegulation = CreateActivateRegulationState(sStartupSequence, sError);
        QState *sShutdownSequence = new QState(sSuperState);
            QState *sDeactivateRegulation = CreateDeactivateRegulationState(sShutdownSequence, sError);
            QState *sSetShutdownInitialNiveau = CreateSetShutdownInitialNiveau(sShutdownSequence, sError);
            QState *sUpFrequency = CreateUpFrequencyState(sShutdownSequence, sError);
            QState *sManualMembranes = CreateManuelMembranesState(sShutdownSequence, sError);
            QState *sSwitchOffNiveaux = CreateSwitchOffNiveauxState(sShutdownSequence, sError);
            QState *sShutdownPowerSupplies = CreateShutdownPowerSuppliesState(sShutdownSequence, sError);
            QState *sPrePositionMembranes = CreatePrepositionMembranesState(sShutdownSequence, sError);

    // SuperState
    sSuperState->setInitialState(sIdle);
    sSuperState->addTransition(this, &HFController::SIGNAL_Startup, sStartupSequence);
    sSuperState->addTransition(this, &HFController::SIGNAL_Shutdown, sShutdownSequence);
    sSuperState->addTransition(this, &HFController::SIGNAL_Interrupt, sIdle);
    sSuperState->addTransition(this, &HFController::SIGNAL_Error, sError);

        // Idle
        // No actions

        // Error
        QObject::connect(sError, &QState::entered, this, [&]() {
            ErrorMessage("Une erreur s'est produite, tous les processus ont été arrêtés");
            Interrupt();
        });
        sError->addTransition(new QSignalTransition(this, &HFController::SIGNAL_Interrupt)); // Ignore signal in error state
        sError->addTransition(new QSignalTransition(this, &HFController::SIGNAL_Error)); // Ignore signal in error state

        // StartupSequence
        sStartupSequence->setInitialState(sSetCleanInitialState);

            // SetCleanInitialState
            sSetCleanInitialState->addTransition(sSetCleanInitialState, &QState::finished, sStartupPowerSupplies);

            // StartupPowerSupplies (if not already started)
            sStartupPowerSupplies->addTransition(sStartupPowerSupplies, &QState::finished, sSwitchOnNiveaux);

            // SwitchOnNiveaux (if not already switched on)
            sSwitchOnNiveaux->addTransition(sSwitchOnNiveaux, &QState::finished, sAccordMembranes);

            // AccordMembranes (if not already accorded)
//            sAccordMembranes->addTransition(sAccordMembranes, &QState::finished, sIdle);
            sAccordMembranes->addTransition(sAccordMembranes, &QState::finished, sDownFrequency);

            // DownFrequency
            sDownFrequency->addTransition(sDownFrequency, &QState::finished, sActivateRegulation);

            // ActivateRegulation
            auto tStartupFinished = sActivateRegulation->addTransition(sActivateRegulation, &QState::finished, sIdle);
            QObject::connect(tStartupFinished, &QSignalTransition::triggered, this, [&]() {
                InfoMessage("Démarrage términé");
                emit SIGNAL_StartupFinished();
            });

        // ShutdownSequence
        sShutdownSequence->setInitialState(sDeactivateRegulation);

            // DeactivateRegulation
            sDeactivateRegulation->addTransition(sDeactivateRegulation, &QState::finished, sSetShutdownInitialNiveau);

            // SetShutdownInitialNiveau
            sSetShutdownInitialNiveau->addTransition(sSetShutdownInitialNiveau, &QState::finished, sUpFrequency);

            // UpFrequency
            sUpFrequency->addTransition(sUpFrequency, &QState::finished, sManualMembranes);

            // ManualMembranes
            sManualMembranes->addTransition(sManualMembranes, &QState::finished, sSwitchOffNiveaux);

            // SwitchOffNiveaux
            sSwitchOffNiveaux->addTransition(sSwitchOffNiveaux, &QState::finished, sShutdownPowerSupplies);

            // ShutdownPowerSupplies
            sShutdownPowerSupplies->addTransition(sShutdownPowerSupplies, &QState::finished, sPrePositionMembranes);

            // PrepositionMembranes
            auto tShutdownFinished = sPrePositionMembranes->addTransition(sPrePositionMembranes, &QState::finished, sIdle);
            QObject::connect(tShutdownFinished, &QSignalTransition::triggered, this, [&]() {
                InfoMessage("Arrêt términé");
                emit SIGNAL_ShutdownFinished();
            });

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sError, "Error");
    PrintStateChanges(sSetCleanInitialState, "SetCleanInitialState");
    PrintStateChanges(sStartupSequence, "StartupSequence");
    PrintStateChanges(sStartupPowerSupplies, "StartupPowerSupplies");
    PrintStateChanges(sSwitchOnNiveaux, "SwitchOnNiveaux");
    PrintStateChanges(sAccordMembranes, "AccordMembranes");
    PrintStateChanges(sDownFrequency, "DownFrequency");
    PrintStateChanges(sActivateRegulation, "ActivateRegulation");
    PrintStateChanges(sDeactivateRegulation, "DeactivateRegulation");
    PrintStateChanges(sSetShutdownInitialNiveau, "SetShutdownInitialNiveau");
    PrintStateChanges(sUpFrequency, "UpFrequency");
    PrintStateChanges(sManualMembranes, "ManualMembranes");
    PrintStateChanges(sSwitchOffNiveaux, "SwitchOffNiveaux");
    PrintStateChanges(sShutdownPowerSupplies, "ShutdownPowerSupplies");
    PrintStateChanges(sPrePositionMembranes, "PrepositionMembranes");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

QState* HFController::CreateSetCleanInitialState(QState *parent, QState *error) {
    // Check if the startup should start from the very beginning, and if so,
    // make sure that it starts from a clean state

    const int CHECK_TIMEOUT = 10000;
    const int SET_TIMEOUT = 10000;
    QState *sSuperState = new QState(parent);
        global::TimedState *sCheckInitialState = new global::TimedState(sSuperState, 15000, "Timeout CheckInitialState", error);
            global::TimedState *sCheckAlims = new global::TimedState(sCheckInitialState, CHECK_TIMEOUT, "Timeout CheckAlims", error);
            global::TimedState *sCheckNiveau1 = new global::TimedState(sCheckInitialState, CHECK_TIMEOUT, "Timeout CheckNiveau1", error);
            global::TimedState *sCheckNiveau2 = new global::TimedState(sCheckInitialState, CHECK_TIMEOUT, "Timeout CheckNiveau2", error);
            global::TimedState *sCheckAccord1 = new global::TimedState(sCheckInitialState, CHECK_TIMEOUT, "Timeout CheckAccord1", error);
            global::TimedState *sCheckAccord2 = new global::TimedState(sCheckInitialState, CHECK_TIMEOUT, "Timeout CheckAccord2", error);
        global::TimedState *sSetCleanState = new global::TimedState(sSuperState, 30000, "Timeout commander un état initial propre", error);
            // In set off the niveau will be adjusted to the off value, which can take a few seconds -> increase timeout slightly
            global::TimedState *sSetNiveauOff1 = new global::TimedState(sSetCleanState, 15000, "Timeout SetNiveauOff1", error);
            global::TimedState *sSetNiveauOff2 = new global::TimedState(sSetCleanState, 15000, "Timeout SetNiveauOff2", error);
            global::TimedState *sSetMembranesManual1 = new global::TimedState(sSetCleanState, SET_TIMEOUT, "Timeout SetMembranesManual1", error);
            global::TimedState *sSetMembranesManual2 = new global::TimedState(sSetCleanState, SET_TIMEOUT, "Timeout SetMembranesManual2", error);
            global::TimedState *sSetRegulationOff1 = new global::TimedState(sSetCleanState, SET_TIMEOUT, "Timeout SetRegulationOff1", error);
            global::TimedState *sSetRegulationOff2 = new global::TimedState(sSetCleanState, SET_TIMEOUT, "Timeout SetRegulationOff2", error);
            global::TimedState *sSetFrequencyCold = new global::TimedState(sSetCleanState, SET_TIMEOUT, "Timeout SetFrequencyCold", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sCheckInitialState);

    // CheckInitialState
    sCheckInitialState->setInitialState(sCheckAlims);
    QObject::connect(sCheckInitialState, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);

        // In below states, a Ping is done on entering in each state to force a rapid reply. Otherwise, the required
        // info is send at 0.5 Hz (in most cases)

        // CheckAlims
        QObject::connect(sCheckAlims, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        // QObject::connect(sCheckAlims, &QState::entered, &powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::Ping);
        auto tAllOn = sCheckAlims->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_True , sCheckNiveau1);
        auto tAllNotOn = sCheckAlims->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_False, sSetCleanState);
        QObject::connect(tAllOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Toutes les alimentations sont allumées"); });
        QObject::connect(tAllNotOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Toutes les alimentations ne sont pas allumées"); });

        // CheckNiveau1
        QObject::connect(sCheckNiveau1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sCheckNiveau1, &QState::entered, niveaux_[HFX::HF1], &Niveau::Ping);
        auto tNiveau1On = sCheckNiveau1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_On, sCheckNiveau2);
        auto tNiveau1Off = sCheckNiveau1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_Off, sSetCleanState);
        QObject::connect(tNiveau1On, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF1 On"); });
        QObject::connect(tNiveau1Off, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF1 Off"); });

        // CheckNiveau2
        QObject::connect(sCheckNiveau2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sCheckNiveau2, &QState::entered, niveaux_[HFX::HF2], &Niveau::Ping);
        auto tNiveau2On = sCheckNiveau2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_On, sCheckAccord1);
        auto tNiveau2Off = sCheckNiveau2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_Off, sSetCleanState);
        QObject::connect(tNiveau2On, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF2 On"); });
        QObject::connect(tNiveau2Off, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF2 Off"); });

        // CheckAccord1
        QObject::connect(sCheckAccord1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sCheckAccord1, &QState::entered, accords_[HFX::HF1], &Accord::Ping);
        auto tHF1Accorded = sCheckAccord1->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_Accorded, sCheckAccord2);
        auto tHF1NotAccorded = sCheckAccord1->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_NotAccorded, sSetMembranesManual1);
        QObject::connect(tHF1Accorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF1 accordé"); });
        QObject::connect(tHF1NotAccorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF1 pas accordé"); });

        // CheckAccord2
        QObject::connect(sCheckAccord2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sCheckAccord2, &QState::entered, accords_[HFX::HF2], &Accord::Ping);
        auto tHF2Accorded = sCheckAccord2->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_Accorded, sDone);
        auto tHF2NotAccorded = sCheckAccord2->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_NotAccorded, sSetMembranesManual2);
        QObject::connect(tHF2Accorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF2 accordé"); });
        QObject::connect(tHF2NotAccorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF2 pas accordé"); });

    // SetCleanState
    sSetCleanState->setInitialState(sSetNiveauOff1);
    QObject::connect(sSetCleanState, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);

        // SetNiveauOff1
        QObject::connect(sSetNiveauOff1, &global::TimedState::entered, this, [&]() { DebugMessage("Désactiver le niveau HF1"); });
        QObject::connect(sSetNiveauOff1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetNiveauOff1, &QState::entered, niveaux_[HFX::HF1], &Niveau::Off);
        sSetNiveauOff1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_Off, sSetNiveauOff2);

        // SetNiveauOff2
        QObject::connect(sSetNiveauOff2, &global::TimedState::entered, this, [&]() { DebugMessage("Désactiver le niveau HF2"); });
        QObject::connect(sSetNiveauOff2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetNiveauOff2, &QState::entered, niveaux_[HFX::HF2], &Niveau::Off);
        sSetNiveauOff2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_Off, sSetMembranesManual1);

        // SetMembranesManual1
        QObject::connect(sSetMembranesManual1, &global::TimedState::entered, this, [&]() { DebugMessage("Régler le mode membrane HF1 sur manuel"); });
        QObject::connect(sSetMembranesManual1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetMembranesManual1, &QState::entered, membranes_[HFX::HF1], &Membrane::SetModeManual);
        sSetMembranesManual1->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_Manual, sSetMembranesManual2);

        // SetMembranesManual2
        QObject::connect(sSetMembranesManual2, &global::TimedState::entered, this, [&]() { DebugMessage("Régler le mode membrane HF2 sur manuel"); });
        QObject::connect(sSetMembranesManual2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetMembranesManual2, &QState::entered, membranes_[HFX::HF2], &Membrane::SetModeManual);
        sSetMembranesManual2->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_Manual, sSetRegulationOff1);

        // SetRegulationOff1
        QObject::connect(sSetRegulationOff1, &global::TimedState::entered, this, [&]() { DebugMessage("Désactiver la régulation HF1"); });
        QObject::connect(sSetRegulationOff1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetRegulationOff1, &QState::entered, dees_[HFX::HF1], &Dee::SetRegulationOff);
        sSetRegulationOff1->addTransition(dees_[HFX::HF1], &Dee::SIGNAL_RegulationOff, sSetRegulationOff2);

        // SetRegulationOff2
        QObject::connect(sSetRegulationOff2, &global::TimedState::entered, this, [&]() { DebugMessage("Désactiver la régulation HF2"); });
        QObject::connect(sSetRegulationOff2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetRegulationOff2, &QState::entered, dees_[HFX::HF2], &Dee::SetRegulationOff);
        sSetRegulationOff2->addTransition(dees_[HFX::HF2], &Dee::SIGNAL_RegulationOff, sSetFrequencyCold);

        // SetFrequencyCold
        QObject::connect(sSetFrequencyCold, &global::TimedState::entered, this, [&]() { DebugMessage("Régler la fréquence à froid"); });
        QObject::connect(sSetFrequencyCold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetFrequencyCold, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::SetFrequencyCold);
        sSetFrequencyCold->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyCold, sDone);

    // Done
    // No more actions

    PrintStateChanges(sCheckInitialState, "CheckInitialState");
    PrintStateChanges(sCheckAlims, "CheckAlims");
    PrintStateChanges(sCheckNiveau1, "CheckNiveau1");
    PrintStateChanges(sCheckNiveau2, "CheckNiveau2");
    PrintStateChanges(sCheckAccord1, "CheckAccord1");
    PrintStateChanges(sCheckAccord2, "CheckAccord2");
    PrintStateChanges(sSetCleanState, "SetCleanState");
    PrintStateChanges(sSetMembranesManual1, "SetMembranesManual1");
    PrintStateChanges(sSetMembranesManual2, "SetMembranesManual2");
    PrintStateChanges(sSetNiveauOff1, "SetNiveauOff1");
    PrintStateChanges(sSetNiveauOff2, "SetNiveauOff2");
    PrintStateChanges(sSetRegulationOff1, "SetRegulationOff1");
    PrintStateChanges(sSetRegulationOff2, "SetRegulationOff2");
    PrintStateChanges(sSetFrequencyCold, "SetFrequencyCold");

    return sSuperState;
}


QState* HFController::CreateStartupPowerSuppliesState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sVerifyActualState = new global::TimedState(sSuperState, 10000, "Timeout VerifyActualState", error);
        global::TimedState *sStartup = new global::TimedState(sSuperState, 4 * 60000, "Timeout démarrage des (pré)amplis", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sVerifyActualState);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Vérifier l'état des alimentations"); });

        // VerifyActualState
        QObject::connect(sVerifyActualState, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
//        QObject::connect(sVerifyActualState, &QState::entered, &powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::Ping);
        sVerifyActualState->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_True, sDone);
        sVerifyActualState->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_False, sStartup);

        // Startup
        QObject::connect(sStartup, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sStartup, &QState::entered, &powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::Startup);
        sStartup->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOn_True, sDone);

        // Done
        QObject::connect(sDone, &QState::entered, this, [&]() { DebugMessage("Les alimentations sont démarrées"); });

    PrintStateChanges(sVerifyActualState, "PowerSupplies::VerifyActualState");
    PrintStateChanges(sStartup, "PowerSupplies::Startup");
    PrintStateChanges(sDone, "PowerSupplies::Done");

    return sSuperState;
}

QState* HFController::CreateShutdownPowerSuppliesState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sVerifyActualState = new global::TimedState(sSuperState, 10000, "Timeout VerifyActualState", error);
        global::TimedState *sShutdown = new global::TimedState(sSuperState, 120000, "Timeout Startup", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sVerifyActualState);

        // VerifyActualState
        QObject::connect(sVerifyActualState, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        sVerifyActualState->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOff_True, sDone);
        sVerifyActualState->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOff_False, sShutdown);

        // shutdown
        QObject::connect(sShutdown, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sShutdown, &QState::entered, &powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::Shutdown);
        sShutdown->addTransition(&powersupplies_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_AllOff_True, sDone);

        // Done
        QObject::connect(sDone, &QState::entered, this, [&]() { DebugMessage("Les alimentations sont coupées"); });

    PrintStateChanges(sVerifyActualState, "PowerSupplies::VerifyActualState");
    PrintStateChanges(sShutdown, "PowerSupplies::Shutdown");
    PrintStateChanges(sDone, "PowerSupplies::Done");

    return sSuperState;
}


QState* HFController::CreateSwitchOnNiveauxState(QState *parent, QState *error) {
    // Switch on each niveau if not already switched on, in that case leave it
    // at the level it is currently at
    QState *sSuperState = new QState(parent);
        global::TimedState *sVerifyHF1 = new global::TimedState(sSuperState, 10000, "Timeout VerifyHF1", error);
        global::TimedState *sSwitchOnHF1 = new global::TimedState(sSuperState, 20000, "Timeout SwitchOnHF1", error);
        global::TimedState *sVerifyHF2 = new global::TimedState(sSuperState, 10000, "Timeout VerifyHF2", error);
        global::TimedState *sSwitchOnHF2 = new global::TimedState(sSuperState, 20000, "Timeout SwitchOnHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sVerifyHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Activation des deux niveaux"); });

        // VerifyHF1
        QObject::connect(sVerifyHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::Ping);
        auto tHF1On = sVerifyHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_On, sVerifyHF2);
        auto tHF1Off = sVerifyHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_Off, sSwitchOnHF1);
        QObject::connect(tHF1On, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF1 déjà On"); });
        QObject::connect(tHF1Off, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF1 désactivé, sera allumé"); });

        // SwitchOnHF1
        QObject::connect(sSwitchOnHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSwitchOnHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::On);
        sSwitchOnHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_On, sVerifyHF2);

        // VerifyHF2
        QObject::connect(sVerifyHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::Ping);
        auto tHF2On = sVerifyHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_On, sDone);
        auto tHF2Off = sVerifyHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_Off, sSwitchOnHF2);
        QObject::connect(tHF2On, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF2 déjà On"); });
        QObject::connect(tHF2Off, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau HF2 désactivé, sera allumé"); });

        // SwitchOnHF2
        QObject::connect(sSwitchOnHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSwitchOnHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::On);
        sSwitchOnHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_On, sDone);

        // Done
        QObject::connect(sDone, &QState::entered, this, [&]() { DebugMessage("Les deux niveaux sont activés"); });

    PrintStateChanges(sVerifyHF1, "Niveau::VerifyHF1");
    PrintStateChanges(sSwitchOnHF1, "Niveau::SwitchOnHF1");
    PrintStateChanges(sVerifyHF2, "Niveau::VerifyHF2");
    PrintStateChanges(sSwitchOnHF2, "Niveau::SwitchOnHF2");
    PrintStateChanges(sDone, "Niveau::Done");

    return sSuperState;
}

QState* HFController::CreateAccordMembranesState(QState *parent, QState *error) {
    // If the two membranes are not accorded, do it and set both dee voltages to the same value (20 kV). If both are
    // already accorded, leave it as it is as we are probably continuing a previously interrupted startup procedure.
    QState *sSuperState = new QState(parent);
        global::TimedState *sVerifyHF1 = new global::TimedState(sSuperState, 10000, "Timeout VerifyHF1", error);
        global::TimedState *sAccordHF1 = new global::TimedState(sSuperState, 3*60000, "Timeout AccordHF1", error);
        global::TimedState *sVerifyHF2 = new global::TimedState(sSuperState, 10000, "Timeout VerifyHF2", error);
        global::TimedState *sAccordHF2 = new global::TimedState(sSuperState, 3*60000, "Timeout AccordHF2", error);
        global::TimedState *sSetVHF1 = new global::TimedState(sSuperState, 30000, "Timeout SetVHF1", error);
        global::TimedState *sSetVHF2 = new global::TimedState(sSuperState, 30000, "Timeout SetVHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

        // SuperState
        sSuperState->setInitialState(sVerifyHF1);
        QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Procédure pour accorder les deux membranes commencée"); });

            // VerifyHF1
            // If the accord module is in the process of finding accord, the below will timeout since a signal wont be sent
            QObject::connect(sVerifyHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            auto tHF1Accorded = sVerifyHF1->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_Accorded, sVerifyHF2);
            auto tHF1NotAccorded = sVerifyHF1->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_NotAccorded, sAccordHF1);
            QObject::connect(tHF1Accorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF1 déjà accordé"); });
            QObject::connect(tHF1NotAccorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF1 non accordé, procédure lancée"); });

            // AccordHF1
            QObject::connect(sAccordHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sAccordHF1, &QState::entered, accords_[HFX::HF1], &Accord::FindAccord);
            sAccordHF1->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_Accorded, sVerifyHF2);

            // VerifyHF2
            QObject::connect(sVerifyHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            auto tHF2Accorded = sVerifyHF2->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_Accorded, sDone);
            auto tHF2NotAccorded = sVerifyHF2->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_NotAccorded, sAccordHF2);
            QObject::connect(tHF2Accorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF2 déjà accordé"); });
            QObject::connect(tHF2NotAccorded, &QSignalTransition::triggered, this, [&]() { DebugMessage("HF2 non accordé, procédure lancée"); });

            // AccordHF2
            QObject::connect(sAccordHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sAccordHF2, &QState::entered, accords_[HFX::HF2], &Accord::FindAccord);
            sAccordHF2->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_Accorded, sSetVHF1);

            // SetVHF1
            QObject::connect(sSetVHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sSetVHF1, &QState::entered, this, [&]() { v_regulators_[HFX::HF1]->SetVoltage(voltage_post_accord_[HFX::HF1]); });
            sSetVHF1->addTransition(v_regulators_[HFX::HF1], &VoltageRegulator::SIGNAL_VoltageSet, sSetVHF2);

            // SetVHF2
            QObject::connect(sSetVHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sSetVHF2, &QState::entered, this, [&]() { v_regulators_[HFX::HF2]->SetVoltage(voltage_post_accord_[HFX::HF2]); });
            sSetVHF2->addTransition(v_regulators_[HFX::HF2], &VoltageRegulator::SIGNAL_VoltageSet, sDone);

            // Done
            QObject::connect(sDone, &QState::entered, this, [&]() { InfoMessage("Les deux membranes sont accordées"); });

        PrintStateChanges(sVerifyHF1, "Accord::VerifyHF1");
        PrintStateChanges(sAccordHF1, "Accord::SwitchOnHF1");
        PrintStateChanges(sVerifyHF2, "Accord::VerifyHF2");
        PrintStateChanges(sAccordHF2, "Accord::SwitchOnHF2");
        PrintStateChanges(sSetVHF1, "Accord::SetVHF1");
        PrintStateChanges(sSetVHF2, "Accord::SetVHF2");
        PrintStateChanges(sDone, "Accord::Done");

        return sSuperState;
}

QState* HFController::CreateDownFrequencyState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sDecrementFrequency = new global::TimedState(sSuperState, 10000, "Timeout DecrementFrequency", error);
        global::TimedState *sVerifyFrequency = new global::TimedState(sSuperState, 10000, "Timeout VerifyFrequency", error);
        QState *sVerifyMembranePositions = new QState(sSuperState);
            global::TimedState *sVerifyMembrane1Position = new global::TimedState(sVerifyMembranePositions, 10000, "Timeout VerifyMembrane1Position", error);
            global::TimedState *sVerifyMembrane2Position = new global::TimedState(sVerifyMembranePositions, 10000, "Timeout VerifyMembrane2Position", error);
            global::TimedState *sWaitMembranesReact = new global::TimedState(sVerifyMembranePositions, 1800, "Waiting WaitMembranesReact", sVerifyMembrane1Position);
        QState *sIncrementNiveaux = new QState(sSuperState);
            global::TimedState *sIncrementNiveau1 = new global::TimedState(sIncrementNiveaux, 30000, "Timeout IncrementNiveau1", error);
            global::TimedState *sIncrementNiveau2 = new global::TimedState(sIncrementNiveaux, 30000, "Timeout IncrementNiveau2", error);
        QState *sWaitMembranesBelowThreshold = new QState(sSuperState);
            global::TimedState *sWaitMembrane1BelowThreshold = new global::TimedState(sWaitMembranesBelowThreshold, 300000, "La membrane HF1 ne s'éloigne pas suffisamment de la limite", error);
            global::TimedState *sWaitMembrane2BelowThreshold = new global::TimedState(sWaitMembranesBelowThreshold, 300000, "La membrane HF2 ne s'éloigne pas suffisamment de la limite", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sVerifyFrequency);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Procédure pour baisser la fréquence de travail commencée"); });


    // DecrementFrequency
    QObject::connect(sDecrementFrequency, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
    QObject::connect(sDecrementFrequency, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::DecrementFrequency);
    sDecrementFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_Frequency, sVerifyFrequency);

    // VerifyFrequency
    QObject::connect(sVerifyFrequency, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
    QObject::connect(sVerifyFrequency, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::Ping);
    auto tWarm = sVerifyFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyWarm, sDone);
    sVerifyFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyAboveWarm, sVerifyMembranePositions);
    QObject::connect(tWarm, &QSignalTransition::triggered, this, [&]() { InfoMessage("Fréquence chaude atteinte"); });

    // VerifyMembranePositions
    sVerifyMembranePositions->setInitialState(sWaitMembranesReact);

        // WaitMembranesReact
        // No more actions

        // VerifyMembrane1Position
        QObject::connect(sVerifyMembrane1Position, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyMembrane1Position, &QState::entered, membranes_[HFX::HF1], &Membrane::Ping);
        auto tMembraneHF1Limit = sVerifyMembrane1Position->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_SWUpperLimitReached, sIncrementNiveaux);
        sVerifyMembrane1Position->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_SWUpperLimitNotReached, sVerifyMembrane2Position);
        QObject::connect(tMembraneHF1Limit, &QSignalTransition::triggered, this, [&]() { DebugMessage("La membrane HF1 a atteint la limite"); });

        // VerifyMembrane2Position
        QObject::connect(sVerifyMembrane2Position, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyMembrane2Position, &QState::entered, membranes_[HFX::HF2], &Membrane::Ping);
        auto tMembraneHF2Limit = sVerifyMembrane2Position->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_SWUpperLimitReached, sIncrementNiveaux);
        sVerifyMembrane2Position->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_SWUpperLimitNotReached, sDecrementFrequency);
        QObject::connect(tMembraneHF2Limit, &QSignalTransition::triggered, this, [&]() { DebugMessage("La membrane HF2 a atteint la limite"); });

    // IncrementNiveaux
    sIncrementNiveaux->setInitialState(sIncrementNiveau1);

        // IncrementNiveau1
        QObject::connect(sIncrementNiveau1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sIncrementNiveau1, &QState::entered, this, [&]() { v_regulators_[HFX::HF1]->IncrementVoltage(voltage_dUp_); });
        sIncrementNiveau1->addTransition(v_regulators_[HFX::HF1], &VoltageRegulator::SIGNAL_VoltageSet, sIncrementNiveau2);
        sIncrementNiveau1->addTransition(v_regulators_[HFX::HF1], &VoltageRegulator::SIGNAL_SetPointAboveMax, sIncrementNiveau2);

        // IncrementNiveau2
        QObject::connect(sIncrementNiveau2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sIncrementNiveau2, &QState::entered, this, [&]() { v_regulators_[HFX::HF2]->IncrementVoltage(voltage_dUp_); });
        sIncrementNiveau2->addTransition(v_regulators_[HFX::HF2], &VoltageRegulator::SIGNAL_VoltageSet, sWaitMembranesBelowThreshold);
        sIncrementNiveau2->addTransition(v_regulators_[HFX::HF2], &VoltageRegulator::SIGNAL_SetPointAboveMax, sWaitMembranesBelowThreshold);

   // WaitMembranesBelowThreshold
   sWaitMembranesBelowThreshold->setInitialState(sWaitMembrane1BelowThreshold);
   QObject::connect(sWaitMembranesBelowThreshold, &QState::entered, this, [&]() { DebugMessage("Attendre que les membranes s'éloignent de la limite"); });

        // WaitMembrane1BelowThreshold
        QObject::connect(sWaitMembrane1BelowThreshold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        sWaitMembrane1BelowThreshold->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_ClearedFromLimits, sWaitMembrane2BelowThreshold);

        // WaitMembrane2BelowThreshold
        QObject::connect(sWaitMembrane2BelowThreshold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        auto tMembranesCleared = sWaitMembrane2BelowThreshold->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_ClearedFromLimits, sDecrementFrequency);
        QObject::connect(tMembranesCleared, &QSignalTransition::triggered, this, [&]() { DebugMessage("Membranes dégagées de la limite, continuez à diminuer la fréquence"); });

   PrintStateChanges(sDecrementFrequency, "DecrementFrequency");
   PrintStateChanges(sVerifyFrequency, "VerifyFrequency");
   PrintStateChanges(sVerifyMembranePositions, "VerifyMembranePositions");
   PrintStateChanges(sVerifyMembrane1Position, "VerifyMembrane1Position");
   PrintStateChanges(sVerifyMembrane2Position, "VerifyMembrane2Position");
   PrintStateChanges(sWaitMembranesReact, "WaitMembranesReact");
   PrintStateChanges(sIncrementNiveaux, "IncrementNiveaux");
   PrintStateChanges(sIncrementNiveau1, "IncrementNiveau1");
   PrintStateChanges(sIncrementNiveau2, "IncrementNiveau2");
   PrintStateChanges(sWaitMembranesBelowThreshold, "WaitMembranesBelowThreshold");
   PrintStateChanges(sWaitMembrane1BelowThreshold, "WaitMembrane1BelowThreshold");
   PrintStateChanges(sWaitMembrane2BelowThreshold, "WaitMembrane2BelowThreshold");
   PrintStateChanges(sDone, "Done");

   return sSuperState;
}

QState* HFController::CreateActivateRegulationState(QState *parent, QState *error) {
    // Set niveaux to preprogrammed levels and activate regulation
    QState *sSuperState = new QState(parent);
        global::TimedState *sSetNiveauHF1 = new global::TimedState(sSuperState, 20000, "Timeout SetNiveauHF1", error);
        global::TimedState *sSetNiveauHF2 = new global::TimedState(sSuperState, 20000, "Timeout SetNiveauHF2", error);
        global::TimedState *sActivateRegulationHF1 = new global::TimedState(sSuperState, 10000, "Timeout ActivateRegulationHF1", error);
        global::TimedState *sActivateRegulationHF2 = new global::TimedState(sSuperState, 10000, "Timeout ActivateRegulationHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sSetNiveauHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Activer la régulation"); });

        // SetNiveauHF1
        QObject::connect(sSetNiveauHF1, &global::TimedState::entered, this,  [&]() { DebugMessage("Augmenter le niveau HF1 jusqu'à la consigne finale"); });
        QObject::connect(sSetNiveauHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetNiveauHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::SetStartupFinalLevel);
        sSetNiveauHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_ValueReached, sSetNiveauHF2);

        // SetNiveauHF2
        QObject::connect(sSetNiveauHF2, &global::TimedState::entered, this,  [&]() { DebugMessage("Augmenter le niveau HF2 jusqu'à la consigne finale"); });
        QObject::connect(sSetNiveauHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSetNiveauHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::SetStartupFinalLevel);
        sSetNiveauHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_ValueReached , sActivateRegulationHF1);

        // ActivateRegulationHF1
        QObject::connect(sActivateRegulationHF1, &QState::entered, dees_[HFX::HF1], &Dee::SetRegulationOn);
        sActivateRegulationHF1->addTransition(dees_[HFX::HF1], &Dee::SIGNAL_RegulationOn, sActivateRegulationHF2);

        // ActivateRegulationHF2
        QObject::connect(sActivateRegulationHF2, &QState::entered, dees_[HFX::HF2], &Dee::SetRegulationOn);
        sActivateRegulationHF2->addTransition(dees_[HFX::HF2], &Dee::SIGNAL_RegulationOn, sDone);

        // Done
        // no actions

    PrintStateChanges(sSetNiveauHF1, "SetNiveauHF1");
    PrintStateChanges(sSetNiveauHF2, "SetNiveauHF2");
    PrintStateChanges(sActivateRegulationHF1, "ActivateRegulationHF1");
    PrintStateChanges(sActivateRegulationHF2, "ActivateRegulationHF2");
    PrintStateChanges(sDone, "Done");

    return sSuperState;
}

QState* HFController::CreateDeactivateRegulationState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sDeactivateRegulationHF1 = new global::TimedState(sSuperState, 10000, "Timeout DeactivateRegulationHF1", error);
        global::TimedState *sDeactivateRegulationHF2 = new global::TimedState(sSuperState, 10000, "Timeout DeactivateRegulationHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
     sSuperState->setInitialState(sDeactivateRegulationHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Deactiver la régulation"); });


        // DeactivateRegulationHF1
        QObject::connect(sDeactivateRegulationHF1, &QState::entered, dees_[HFX::HF1], &Dee::SetRegulationOff);
        sDeactivateRegulationHF1->addTransition(dees_[HFX::HF1], &Dee::SIGNAL_RegulationOff, sDeactivateRegulationHF2);

        // DeactivateRegulationHF2
        QObject::connect(sDeactivateRegulationHF2, &QState::entered, dees_[HFX::HF2], &Dee::SetRegulationOff);
        sDeactivateRegulationHF2->addTransition(dees_[HFX::HF2], &Dee::SIGNAL_RegulationOff, sDone);

        // Done
        // no actions

    PrintStateChanges(sDeactivateRegulationHF1, "DeactivateRegulationHF1");
    PrintStateChanges(sDeactivateRegulationHF2, "DeactivateRegulationHF2");
    PrintStateChanges(sDone, "Done");

    return sSuperState;
}

QState* HFController::CreateSetShutdownInitialNiveau(QState *parent, QState *error) {
    // If the niveau is above the parametrized value, it is lowered to this value, otherwise nothing is done

    QState *sSuperState = new QState(parent);
        global::TimedState *sCheckActualNiveauHF1 = new global::TimedState(sSuperState, 10000, "Timeout CheckActualNiveauHF1", error);
        global::TimedState *sSetNiveauHF1 = new global::TimedState(sSuperState, 10000, "Timeout SetNiveauHF1", error);
        global::TimedState *sCheckActualNiveauHF2 = new global::TimedState(sSuperState, 10000, "Timeout CheckActualNiveauHF2", error);
        global::TimedState *sSetNiveauHF2 = new global::TimedState(sSuperState, 10000, "Timeout SetNiveauHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sCheckActualNiveauHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Baisser les niveaux"); });


        // CheckActualNiveauHF1
        QObject::connect(sCheckActualNiveauHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::Ping);
        sCheckActualNiveauHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_AboveShutdownInitialValue, sSetNiveauHF1);
        sCheckActualNiveauHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_BelowShutdownInitialValue, sCheckActualNiveauHF2);

        // SetNiveauHF1
        QObject::connect(sSetNiveauHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::SetShutdownInitialLevel);
        sSetNiveauHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_BelowShutdownInitialValue, sCheckActualNiveauHF2);

        // CheckActualNiveauHF2
        QObject::connect(sCheckActualNiveauHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::Ping);
        sCheckActualNiveauHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_AboveShutdownInitialValue, sSetNiveauHF2);
        sCheckActualNiveauHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_BelowShutdownInitialValue, sDone);

        // SetNiveauHF2
        QObject::connect(sSetNiveauHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::SetShutdownInitialLevel);
        sSetNiveauHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_BelowShutdownInitialValue, sDone);

        // Done
        // no actions

    PrintStateChanges(sCheckActualNiveauHF1, "CheckActualNiveauHF1");
    PrintStateChanges(sSetNiveauHF1, "SetNiveauHF1");
    PrintStateChanges(sCheckActualNiveauHF2, "CheckActualNiveauHF2");
    PrintStateChanges(sSetNiveauHF2, "SetNiveauHF2");
    PrintStateChanges(sDone, "Done");

    return sSuperState;
}

QState* HFController::CreateUpFrequencyState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sIncrementFrequency = new global::TimedState(sSuperState, 10000, "Timeout IncrementFrequency", error);
        global::TimedState *sVerifyFrequency = new global::TimedState(sSuperState, 10000, "Timeout VerifyFrequency", error);
        QState *sVerifyMembranePositions = new QState(sSuperState);
            global::TimedState *sVerifyMembrane1Position = new global::TimedState(sVerifyMembranePositions, 10000, "Timeout VerifyMembrane1Position", error);
            global::TimedState *sVerifyMembrane2Position = new global::TimedState(sVerifyMembranePositions, 10000, "Timeout VerifyMembrane2Position", error);
            global::TimedState *sWaitMembranesReact = new global::TimedState(sVerifyMembranePositions, 1800, "Waiting for membranes to react", sVerifyMembrane1Position);
        QState *sDecrementNiveaux = new QState(sSuperState);
            global::TimedState *sDecrementNiveau1 = new global::TimedState(sDecrementNiveaux, 30000, "Timeout DecrementNiveau1", error);
            global::TimedState *sDecrementNiveau2 = new global::TimedState(sDecrementNiveaux, 30000, "Timeout DecrementNiveau2", error);
        QState *sVerifyVoltageThresholdReached = new QState(sSuperState);
            global::TimedState *sVerifyHF1VoltageThresholdReached = new global::TimedState(sVerifyVoltageThresholdReached, 10000, "Timeout VerifyHF1VoltageThresholdReached", error);
            global::TimedState *sVerifyHF2VoltageThresholdReached = new global::TimedState(sVerifyVoltageThresholdReached, 10000, "Timeout VerifyHF2VoltageThresholdReached", error);
        global::TimedState *sSetFrequencyCold = new global::TimedState(sSuperState, 10000, "Timeout réglage fréquence froid", error);
        QState *sWaitMembranesAboveThreshold = new QState(sSuperState);
            global::TimedState *sWaitMembrane1AboveThreshold = new global::TimedState(sWaitMembranesAboveThreshold, 240000, "La membrane 1 ne s'éloigne pas suffisamment de la limite", error);
            global::TimedState *sWaitMembrane2AboveThreshold = new global::TimedState(sWaitMembranesAboveThreshold, 240000, "La membrane 2 ne s'éloigne pas suffisamment de la limite", error);
        QState *sVerifySwitchOffAllowed = new QState(sSuperState);
            global::TimedState *sVerifyHF1SwitchOffAllowed = new global::TimedState(sVerifySwitchOffAllowed, 10000, "Timeout VerifyHF1SwitchOffAllowed", error);
            global::TimedState *sVerifyHF2SwitchOffAllowed = new global::TimedState(sVerifySwitchOffAllowed, 10000, "Timeout VerifyHF2SwitchOffAllowed", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sVerifyFrequency);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Procédure pour monter la fréquence de travail commencée"); });

    // IncrementFrequency
    QObject::connect(sIncrementFrequency, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
    QObject::connect(sIncrementFrequency, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::IncrementFrequency);
    sIncrementFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_Frequency, sVerifyFrequency);

    // VerifyFrequency
    QObject::connect(sVerifyFrequency, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
    QObject::connect(sVerifyFrequency, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::Ping);
    auto tFrequencyColdReached = sVerifyFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyCold, sVerifySwitchOffAllowed);
    sVerifyFrequency->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyBelowCold, sVerifyMembranePositions);
    QObject::connect(tFrequencyColdReached, &QSignalTransition::triggered, this, [&]() { InfoMessage("Fréquence froid atteint"); });

    // VerifyMembranePositions
    sVerifyMembranePositions->setInitialState(sWaitMembranesReact);

        // WaitMembranesReact
        // No more actions

        // VerifyMembrane1Position
        QObject::connect(sVerifyMembrane1Position, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyMembrane1Position, &QState::entered, membranes_[HFX::HF1], &Membrane::Ping);
        auto tMembraneHF1Limit = sVerifyMembrane1Position->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_SWLowerLimitReached, sDecrementNiveaux);
        sVerifyMembrane1Position->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_SWLowerLimitNotReached, sVerifyMembrane2Position);
        QObject::connect(tMembraneHF1Limit, &QSignalTransition::triggered, this, [&]() { DebugMessage("La membrane HF1 a atteint la limite"); });

        // VerifyMembrane2Position
        QObject::connect(sVerifyMembrane2Position, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyMembrane2Position, &QState::entered, membranes_[HFX::HF2], &Membrane::Ping);
        auto tMembraneHF2Limit = sVerifyMembrane2Position->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_SWLowerLimitReached, sDecrementNiveaux);
        sVerifyMembrane2Position->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_SWLowerLimitNotReached, sIncrementFrequency);
        QObject::connect(tMembraneHF2Limit, &QSignalTransition::triggered, this, [&]() { DebugMessage("La membrane HF2 a atteint la limite"); });

    // DecrementNiveaux
    sDecrementNiveaux->setInitialState(sDecrementNiveau1);

        // DecrementNiveau1
        QObject::connect(sDecrementNiveau1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sDecrementNiveau1, &QState::entered, this, [&]() { v_regulators_[HFX::HF1]->DecrementVoltage(voltage_dDown_); });
        sDecrementNiveau1->addTransition(v_regulators_[HFX::HF1], &VoltageRegulator::SIGNAL_VoltageSet, sDecrementNiveau2);

        // DecrementNiveau2
        QObject::connect(sDecrementNiveau2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sDecrementNiveau2, &QState::entered, this, [&]() { v_regulators_[HFX::HF2]->DecrementVoltage(voltage_dDown_); });
        sDecrementNiveau2->addTransition(v_regulators_[HFX::HF2], &VoltageRegulator::SIGNAL_VoltageSet, sVerifyVoltageThresholdReached);

   // VerifyVoltageThresholdReached
   sVerifyVoltageThresholdReached->setInitialState(sVerifyHF1VoltageThresholdReached);

        // VerifyHF1VoltageThresholdReached
        QObject::connect(sVerifyHF1VoltageThresholdReached, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF1VoltageThresholdReached, &QState::entered, accords_[HFX::HF1], &Accord::Ping);
        sVerifyHF1VoltageThresholdReached->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_VoltageBelowLowerLimit, sVerifyHF2VoltageThresholdReached);
        sVerifyHF1VoltageThresholdReached->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_VoltageAboveLowerLimit, sWaitMembranesAboveThreshold);

        // VerifyHF2VoltageThresholdReached
        QObject::connect(sVerifyHF2VoltageThresholdReached, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF2VoltageThresholdReached, &QState::entered, accords_[HFX::HF2], &Accord::Ping);
        auto tVoltagesBelowThreshold = sVerifyHF2VoltageThresholdReached->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_VoltageBelowLowerLimit, sSetFrequencyCold);
        QObject::connect(tVoltagesBelowThreshold, &QSignalTransition::triggered, this, [&]() { InfoMessage("Les deux tensions HF1, HF2 sont inférieures au seuil, la fréquence sera réglez directement à froid"); });
        sVerifyHF2VoltageThresholdReached->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_VoltageAboveLowerLimit, sWaitMembranesAboveThreshold);

    // SetFrequencyCold
    QObject::connect(sSetFrequencyCold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
    QObject::connect(sSetFrequencyCold, &QState::entered, &generator_, &medicyc::cyclotron::SignalGeneratorInterface::SetFrequencyCold);
    sSetFrequencyCold->addTransition(&generator_, &medicyc::cyclotron::SignalGeneratorInterface::SIGNAL_FrequencyCold, sDone);

   // WaitMembranesAboveThreshold
   sWaitMembranesAboveThreshold->setInitialState(sWaitMembrane1AboveThreshold);

        // WaitMembrane1AboveThreshold
       QObject::connect(sWaitMembrane1AboveThreshold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        sWaitMembrane1AboveThreshold->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_ClearedFromLimits, sWaitMembrane2AboveThreshold);

        // WaitMembrane2AboveThreshold
        QObject::connect(sWaitMembrane2AboveThreshold, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        auto tMembranesClearedFromLimit = sWaitMembrane2AboveThreshold->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_ClearedFromLimits, sIncrementFrequency);
        QObject::connect(tMembranesClearedFromLimit, &QSignalTransition::triggered, this, [&]() { DebugMessage("Membranes dégagées de la limite, continuez à monter la fréquence"); });


    // VerifySwitchOffAllowed
    sVerifySwitchOffAllowed->setInitialState(sVerifyHF1SwitchOffAllowed);

        // sVerifyHF1SwitchOffAllowed
        QObject::connect(sVerifyHF1SwitchOffAllowed, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF1SwitchOffAllowed, &QState::entered, accords_[HFX::HF1], &Accord::Ping);
        sVerifyHF1SwitchOffAllowed->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_VoltageBelowLowerLimit, sVerifyHF2SwitchOffAllowed);
        auto tVoltageRemainsOnHF1 = sVerifyHF1SwitchOffAllowed->addTransition(accords_[HFX::HF1], &Accord::SIGNAL_VoltageAboveLowerLimit, error);
        QObject::connect(tVoltageRemainsOnHF1, &QSignalTransition::triggered, this, [&]() { ErrorMessage("La fréquence est réglée à froid, mais il y a toujours de la tension sur les dees. Résolvez ce problème avant de poursuivre l'arrêt."); });

        // VerifyHF2SwitchOffAllowed
        QObject::connect(sVerifyHF2SwitchOffAllowed, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sVerifyHF2SwitchOffAllowed, &QState::entered, accords_[HFX::HF2], &Accord::Ping);
        sVerifyHF2SwitchOffAllowed->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_VoltageBelowLowerLimit, sDone);
        auto tVoltageRemainsOnHF2 = sVerifyHF2SwitchOffAllowed->addTransition(accords_[HFX::HF2], &Accord::SIGNAL_VoltageAboveLowerLimit, error);
        QObject::connect(tVoltageRemainsOnHF2, &QSignalTransition::triggered, this, [&]() { ErrorMessage("La fréquence est réglée à froid, mais il y a toujours de la tension sur les dees. Résolvez ce problème avant de poursuivre l'arrêt."); });

   PrintStateChanges(sIncrementFrequency, "IncrementFrequency");
   PrintStateChanges(sVerifyFrequency, "VerifyFrequency");
   PrintStateChanges(sVerifyMembranePositions, "VerifyMembranePositions");
   PrintStateChanges(sVerifyMembrane1Position, "VerifyMembrane1Position");
   PrintStateChanges(sVerifyMembrane2Position, "VerifyMembrane2Position");
   PrintStateChanges(sWaitMembranesReact, "WaitMembranesReact");
   PrintStateChanges(sDecrementNiveaux, "DecrementNiveaux");
   PrintStateChanges(sDecrementNiveau1, "DecrementNiveau1");
   PrintStateChanges(sDecrementNiveau2, "DecrementNiveau2");
   PrintStateChanges(sVerifyHF1VoltageThresholdReached, "VerifyHF1VoltageThresholdReached");
   PrintStateChanges(sVerifyHF2VoltageThresholdReached, "VerifyHF2VoltageThresholdReached");
   PrintStateChanges(sSetFrequencyCold, "SetFrequencyCold");
   PrintStateChanges(sWaitMembranesAboveThreshold, "WaitMembranesAboveThreshold");
   PrintStateChanges(sWaitMembrane1AboveThreshold, "WaitMembrane1AboveThreshold");
   PrintStateChanges(sWaitMembrane2AboveThreshold, "WaitMembrane2AboveThreshold");
   PrintStateChanges(sVerifySwitchOffAllowed, "VerifySwitchOffAllowed");
   PrintStateChanges(sDone, "Done");

   return sSuperState;
}

QState* HFController::CreateManuelMembranesState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sManualMembraneHF1 = new global::TimedState(sSuperState, 10000, "Timeout ManualMembraneHF1", error);
        global::TimedState *sManualMembraneHF2 = new global::TimedState(sSuperState, 10000, "Timeout ManualMembraneHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sManualMembraneHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Régler les membranes sur manuel"); });


        // ManualMembraneHF1
        QObject::connect(sManualMembraneHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sManualMembraneHF1, &QState::entered, membranes_[HFX::HF1], &Membrane::SetModeManual);
        sManualMembraneHF1->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_Manual, sManualMembraneHF2);

        // ManualMembraneHF2
        QObject::connect(sManualMembraneHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sManualMembraneHF2, &QState::entered, membranes_[HFX::HF2], &Membrane::SetModeManual);
        sManualMembraneHF2->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_Manual, sDone);

        // Done
        // No actions

    PrintStateChanges(sManualMembraneHF1, "ManualMembraneHF1");
    PrintStateChanges(sManualMembraneHF2, "ManualMembraneHF2");
    PrintStateChanges(sDone, "Done");

    return sSuperState;
}


QState* HFController::CreateSwitchOffNiveauxState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
        global::TimedState *sSwitchOffHF1 = new global::TimedState(sSuperState, 20000, "Timeout SwitchOffHF1", error);
        global::TimedState *sSwitchOffHF2 = new global::TimedState(sSuperState, 20000, "Timeout SwitchOffHF2", error);
        QFinalState *sDone = new QFinalState(sSuperState);

    // SuperState
    sSuperState->setInitialState(sSwitchOffHF1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Désactiver les niveaux"); });

        // SwitchOffHF1
        QObject::connect(sSwitchOffHF1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSwitchOffHF1, &QState::entered, niveaux_[HFX::HF1], &Niveau::Off);
        sSwitchOffHF1->addTransition(niveaux_[HFX::HF1], &Niveau::SIGNAL_Off, sSwitchOffHF2);

        // SwitchOffHF2
        QObject::connect(sSwitchOffHF2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
        QObject::connect(sSwitchOffHF2, &QState::entered, niveaux_[HFX::HF2], &Niveau::Off);
        sSwitchOffHF2->addTransition(niveaux_[HFX::HF2], &Niveau::SIGNAL_Off, sDone);

        // Done
        // No actions

    PrintStateChanges(sSwitchOffHF1, "SwitchOffHF1");
    PrintStateChanges(sSwitchOffHF2, "SwitchOffHF2");
    PrintStateChanges(sDone, "Done");

    return sSuperState;
}

QState* HFController::CreatePrepositionMembranesState(QState *parent, QState *error) {
    QState *sSuperState = new QState(parent);
       global::TimedState *sRepositionMembrane1 = new global::TimedState(sSuperState, 150000, "Timeout RepositionMembrane1", error);
       global::TimedState *sRepositionMembrane2 = new global::TimedState(sSuperState, 150000, "Timeout RepositionMembrane2", error);
       QFinalState *sDone = new QFinalState(sSuperState);

     // SuperState
     sSuperState->setInitialState(sRepositionMembrane1);
    QObject::connect(sSuperState, &QState::entered, this, [&]() { InfoMessage("Positionner les membranes à la dernière position d'accord connue"); });


            // RepositionMembrane1
            QObject::connect(sRepositionMembrane1, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sRepositionMembrane1, &QState::entered, membranes_[HFX::HF1], &Membrane::MoveToAccordPosition);
            auto tAccordReached1 = sRepositionMembrane1->addTransition(membranes_[HFX::HF1], &Membrane::SIGNAL_AccordPositionReached, sRepositionMembrane2);
            QObject::connect(tAccordReached1, &QSignalTransition::triggered, membranes_[HFX::HF1], &Membrane::Stop);

            // RepositionMembrane2
            QObject::connect(sRepositionMembrane2, &global::TimedState::SIGNAL_Timeout, this, &HFController::ErrorMessage);
            QObject::connect(sRepositionMembrane2, &QState::entered, membranes_[HFX::HF2], &Membrane::MoveToAccordPosition);
            auto tAccordReached2 = sRepositionMembrane2->addTransition(membranes_[HFX::HF2], &Membrane::SIGNAL_AccordPositionReached, sDone);
            QObject::connect(tAccordReached2, &QSignalTransition::triggered, membranes_[HFX::HF2], &Membrane::Stop);

            // Done
            // No actions

        PrintStateChanges(sRepositionMembrane1, "RepositionMembrane1");
        PrintStateChanges(sRepositionMembrane2, "RepositionMembrane2");
        PrintStateChanges(sDone, "Done");

    return sSuperState;
}

void HFController::Startup() {
    qDebug() << "HFController::Startup";
    emit SIGNAL_Startup();
}

void HFController::Shutdown() {
    qDebug() << "HFController::Shutdown";
    emit SIGNAL_Shutdown();
}

void HFController::IncNiveau(int hf) {
    HFX hfx = static_cast<medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX>(hf);
    if (niveaux_.contains(hfx)) {
        niveaux_[hfx]->Increment();
    } else {
        qWarning() << "HFController::IncNiveau No such index " << hf;
    }
}

void HFController::DecNiveau(int hf) {
    HFX hfx = static_cast<medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX>(hf);
    if (niveaux_.contains(hfx)) {
        niveaux_[hfx]->Decrement();
    } else {
        qWarning() << "HFController::IncNiveau No such index " << hf;
    }
}

void HFController::IncFrequency() {
    generator_.IncrementFrequency();
}

void HFController::DecFrequency() {
    generator_.DecrementFrequency();
}

void HFController::SetFrequency(double value) {
    generator_.SetFrequency(value);
}

void HFController::InitGenerator() {
    generator_.Init();
}

void HFController::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "HF ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "HF <-" << name; });
}

void HFController::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF1-2", message);
}

void HFController::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF1-2", message);
}

void HFController::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF1-2", message);
}

void HFController::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF1-2", message);
}

} // namespace
