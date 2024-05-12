#include "SourcePowerSupply.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QSignalTransition>
#include <QFinalState>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

SourcePowerSupply::SourcePowerSupply(PowerSupplyConfig config) :
    config_(config),
    voltage_(config.actuated_param_config()),
    regulated_(config.regulated_param_config()),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.source", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    config.print();

    ConnectSignals();
    SetupTimers();
    SetupStateMachine();
}

SourcePowerSupply::~SourcePowerSupply() {

}

// Public interface

void SourcePowerSupply::Configure(double physical_value) {
    qDebug() << "SourcePowerSupply::Configure " << physical_value;
    if (regulated_.AllowedSetPoint(physical_value)) {
        startup_parameters_.physical_value_ = physical_value;
        startup_parameters_.params_set_ = true;
        DebugMessage("Configuration: " + QString::number(physical_value));
        emit SIGNAL_Configuration(physical_value);
    } else {
        qWarning() << "SourcePowerSupply::Configure Outside limits. SetPoint " << physical_value;
        emit SIGNAL_Error("La consigne est en dehors de l'intervalle autorisé");
    }
}

void SourcePowerSupply::Startup() {
    qDebug() << "SourcePowerSupply::StartUp";    
    if (startup_parameters_.params_set_) {
        regulated_.SetDesValue(startup_parameters_.physical_value_);
        emit SIGNAL_StartUp();
    } else {
        emit SIGNAL_Error("L'alimentation n'a pas été configurée avec une consigne");
    }
}

void SourcePowerSupply::Shutdown() {
    qDebug() << "SourcePowerSupply::Shutdown";
    emit SIGNAL_ShutDown();
}

void SourcePowerSupply::SwitchOn() {
    qDebug() << "SourcePowerSupply::SwitchOn";
    emit SIGNAL_SwitchOn();
}

void SourcePowerSupply::SwitchOff() {
    qDebug() << "SourcePowerSupply::SwitchOff";
    emit SIGNAL_SwitchOff();
}

void SourcePowerSupply::Interrupt() {
    qDebug() << "SourcePowerSupply::Interrupt";
    emit SIGNAL_Interrupted("L'alim a été interrompue par l'opérateur");
}

void SourcePowerSupply::UpdateDesiredValue(double value) {
    qDebug() << "SourcePowerSupply::UpdateDesiredValue " << value;
    if (regulated_.AllowedSetPoint(value)) {
        regulated_.SetDesValue(value);
        emit SIGNAL_GotoDesValue();
    } else {
        qWarning() << "SourcePowerSupply::UpdateDesiredValue Outside limits. SetPoint " << value;
        ErrorMessage("La consigne est en dehors de l'intervalle autorisé");
    }
}

void SourcePowerSupply::Increment(int steps) {
    qDebug() << "SourcePowerSupply::Increment " << steps;
    (void)steps;
    emit SIGNAL_Increment();
}

void SourcePowerSupply::Decrement(int steps) {
    qDebug() << "SourcePowerSupply::Decrement " << steps;
    (void)steps;
    emit SIGNAL_Decrement();
}

// Internal functions

void SourcePowerSupply::SetupTimers() {
    qDebug() << "SourcePowerSupply::SetupTimers";
    tPing_.setInterval(1000);
    tPing_.setSingleShot(false);
    QObject::connect(&tPing_, &QTimer::timeout, this, [&]() {
        ReadMode();
        ReadIsOn();
        ReadParams();
        CheckDbus();
    });
}

void SourcePowerSupply::ReadParams() {
    ReadVoltage();
    ReadCurrent();
    // If the regulated param is the same as the actuated (commonly the case), dont read both
    if (config_.regulated_param() != ("V." + config_.name() + " Source")) {
        ReadRegulatedParam();
    }
}

void SourcePowerSupply::SetupStateMachine() {
    qDebug() << "SourcePowerSupply::SetupStateMachine";

    // State declarations
    QState *sSuperState = new QState();
    global::TimedState *sDisconnected = new global::TimedState(sSuperState, 3000, "Check connections");
        global::TimedState *sADCConnection = new global::TimedState(sDisconnected, 3000, "Check ADC connection", sDisconnected);
        global::TimedState *sOmronConnection = new global::TimedState(sDisconnected, 3000, "Check Omron connection", sDisconnected);
    global::TimedState *sDemandForConfiguration = new global::TimedState(sSuperState, 3000, "Timeout demande configuration", sDisconnected);
    QState *sConnected = new QState(sSuperState);
        QState *sCheckMode = new QState(sConnected);
        QState *sLocal = new QState(sConnected);
        QState *sRemote = new QState(sConnected);
         QState *sOff = new QState(sRemote);
             QState *sIdle = new QState(sOff);
             global::TimedState *sStartUp = new global::TimedState(sOff, config_.heat_up_time() + 10000, "Echec du démarrage de l'alim", sIdle);
         global::TimedState *sCheckState = new global::TimedState(sRemote, 10000, "Echec de la vérification de l'état de l'alim", sOff);
             global::TimedState *sReadValue = new global::TimedState(sCheckState, 2000, "Echec de la lecture de la valeur de l'alim");
             global::TimedState *sReadStatus = new global::TimedState(sCheckState, 2000, "Echec de la lecture de l'état de l'alim");
         QState *sOn = new QState(sRemote);
             QState *sHalted = new QState(sOn);
             global::TimedState *sCheckTurnOffAllowed = new global::TimedState(sOn, 3000, "échec de la vérification si l'arrêt est autorisé", sHalted);
             global::TimedState *sRoughRegulation = new global::TimedState(sOn, 30000, QString("La boucle de régulation brut de l'alim a échoué"), sHalted);
             global::TimedState *sFineRegulation = new global::TimedState(sOn, 30000, QString("La boucle de régulation fine de l'alim a échoué"), sHalted);
             global::TimedState *sSetActAsSetPoint = new global::TimedState(sOn, 3000, "Echec du réglage de la valeur souhaitée", sHalted);
             global::TimedState *sIncrement = new global::TimedState(sOn, 3000, "Wait", sSetActAsSetPoint);
             global::TimedState *sDecrement = new global::TimedState(sOn, 3000, "Wait", sSetActAsSetPoint);
             QState *sReady = new QState(sOn);
                 global::TimedState *sCheckOnOffTarget = new global::TimedState(sReady, 30000, "Off target", sHalted);
                 QState *sOffTarget = new QState(sReady);
                 QState *sOnTarget = new QState(sReady);
             global::TimedState *sShutDown = new global::TimedState(sOn, 50000, QString("Timeout de l'arrêt de l'alim"), sHalted);
                 global::TimedState *sRampDown = new global::TimedState(sShutDown, 1500, "Next step");
                 global::TimedState *sSwitchOff = new global::TimedState(sShutDown, 4000, "Echec de l'arrêt de l'alim");
                 global::TimedState *sCoolDown = new global::TimedState(sShutDown, 4000, "Cool down", sSwitchOff);

    // State definitions

    // SuperState
    sSuperState->setInitialState(sDemandForConfiguration);

         // DemandForConfiguration
         QObject::connect(sDemandForConfiguration, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);
         QObject::connect(sDemandForConfiguration, &QState::entered, this, &SourcePowerSupply::SIGNAL_DemandForConfiguration);
         sDemandForConfiguration->addTransition(this, &SourcePowerSupply::SIGNAL_Configuration, sDisconnected);

         // Disconnected
         QObject::connect(sDisconnected, &QState::entered, &tPing_, &QTimer::stop);
         sDisconnected->setInitialState(sADCConnection);
         QSignalTransition *tStartUpInDisconnected = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_StartUp);
         sDisconnected->addTransition(tStartUpInDisconnected);
         QObject::connect(tStartUpInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Démarrage impossible, équipement pas connecté"); });
         QSignalTransition *tShutdownInDisconnected = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_ShutDown);
         sDisconnected->addTransition(tShutdownInDisconnected);
         QObject::connect(tShutdownInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Shutdown impossible, équipement pas connecté"); });
         QSignalTransition *tIncrementInDisconnected = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_Increment);
         sDisconnected->addTransition(tIncrementInDisconnected);
         QObject::connect(tIncrementInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Increment impossible, équipement pas connecté"); });
         QSignalTransition *tDecrementInDisconnected = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_Decrement);
         sDisconnected->addTransition(tDecrementInDisconnected);
         QObject::connect(tDecrementInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Decrement impossible, équipement pas connecté"); });

            // ADCConnection
            QObject::connect(sADCConnection, &global::TimedState::entered, this, [&]() { adc_.Ping(); });
            sADCConnection->addTransition(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_Connected, sOmronConnection);

            // OmronConnection
            QObject::connect(sOmronConnection, &global::TimedState::entered, this, [&]() { omron_.Ping(); });
            sOmronConnection->addTransition(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_Connected, sConnected);

         // Connected
         sConnected->setInitialState(sCheckMode);
         auto tDbusError = sConnected->addTransition(this, &SourcePowerSupply::SIGNAL_DbusError, sDisconnected);
         QObject::connect(tDbusError, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Perte de connexion DBUS"); });
         QObject::connect(sConnected, &QState::entered, this, [&]() { InfoMessage("Connexion établie"); });
         auto tADCDisconnected = sConnected->addTransition(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_Disconnected, sDisconnected);
         QObject::connect(tADCDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Perte de connexion ADC"); });
         auto tOmronDisconnected = sConnected->addTransition(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_Disconnected, sDisconnected);
         QObject::connect(tOmronDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Perte de connexion Omron"); });

             // CheckMode
             QObject::connect(sCheckMode, &QState::entered, this, [&]() { tPing_.start(2000); });
             QObject::connect(sCheckMode, &QState::entered, this, [&]() { ReadMode(); });
             sCheckMode->addTransition(this, &SourcePowerSupply::SIGNAL_Remote, sRemote);
             sCheckMode->addTransition(this, &SourcePowerSupply::SIGNAL_Local, sLocal);

             // Local
             QObject::connect(sLocal, &QState::entered, this, [&]() { tPing_.start(2000); });
             QObject::connect(sLocal, &QState::entered, this, [&]() { emit SIGNAL_Error("Equipement en mode locale"); });
             sLocal->addTransition(this, &SourcePowerSupply::SIGNAL_Remote, sRemote);
             QSignalTransition *tStartUpInLocal = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_StartUp);
             sLocal->addTransition(tStartUpInLocal);
             QObject::connect(tStartUpInLocal, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Démarrage impossible, équipement en mode local"); });
             QSignalTransition *tShutdownInLocal = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_ShutDown);
             sLocal->addTransition(tShutdownInLocal);
             QObject::connect(tShutdownInLocal, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Shutdown impossible, équipement en mode local"); });
             QSignalTransition *tIncrementInLocal = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_Increment);
             sLocal->addTransition(tIncrementInLocal);
             QObject::connect(tIncrementInLocal, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Increment impossible, équipement en mode local"); });
             QSignalTransition *tDecrementInLocal = new QSignalTransition(this, &SourcePowerSupply::SIGNAL_Decrement);
             sLocal->addTransition(tDecrementInLocal);
             QObject::connect(tDecrementInLocal, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Decrement impossible, équipement en mode local"); });

             // Remote
             QObject::connect(sRemote, &QState::entered, this, [&]() { tPing_.stop(); });
             QObject::connect(sRemote, &QState::entered, this, [&]() { WarningMessage("Equipement en mode remote"); });
             sRemote->setInitialState(sCheckState);
             sRemote->addTransition(this, &SourcePowerSupply::SIGNAL_Local, sLocal);

                 // Off
                 sOff->setInitialState(sIdle);
                 sOff->addTransition(this, &SourcePowerSupply::SIGNAL_On, sOn);
                 sOff->addTransition(this, &SourcePowerSupply::SIGNAL_StartUp, sStartUp);
                 auto tCommandOnInOff = sOff->addTransition(this, &SourcePowerSupply::SIGNAL_SwitchOn, sIdle);
                 QObject::connect(tCommandOnInOff, &QSignalTransition::triggered, this, &SourcePowerSupply::WriteOn);
                 auto tCommandOffInOff = sOff->addTransition(this, &SourcePowerSupply::SIGNAL_SwitchOff, sIdle);
                 QObject::connect(tCommandOffInOff, &QSignalTransition::triggered, this, [&]() {
                   DebugMessage("La commande d'arrêt lorsque l'alimentation est coupée est ignorée"); });

                     // Idle
                     QObject::connect(sIdle, &QState::entered, this, [&]() { tPing_.start(3000); });

                    // StartUp
                    QObject::connect(sStartUp, &QState::entered, &tPing_, &QTimer::stop);
                    QObject::connect(sStartUp, &QState::entered, this, [&]() { DebugMessage("L'alim est en cours de démarrage"); });
                    QObject::connect(sStartUp, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);
                    sStartUp->addTransition(this, &SourcePowerSupply::SIGNAL_Interrupted, sIdle);
                    sStartUp->addTransition(sStartUp, &QState::finished, sRoughRegulation);
                    CreateStartupStates(sStartUp);

                 // CheckState
                 QObject::connect(sCheckState, &QState::entered, &tPing_, &QTimer::stop);
                 sCheckState->setInitialState(sReadValue);
                 QObject::connect(sCheckState, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);

                      // ReadValue
                      QObject::connect(sReadValue, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::DebugMessage);
                      QObject::connect(sReadValue, &QState::entered, this, &SourcePowerSupply::ReadParams);
                      sReadValue->addTransition(this, &SourcePowerSupply::SIGNAL_Voltage, sReadStatus);

                     // ReadStatus
                     QObject::connect(sReadStatus, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::DebugMessage);
                     QObject::connect(sReadStatus, &QState::entered, this, &SourcePowerSupply::ReadIsOn);
                     sReadStatus->addTransition(this, &SourcePowerSupply::SIGNAL_On, sOn);
                     sReadStatus->addTransition(this, &SourcePowerSupply::SIGNAL_Off, sOff);

             // On
             sOn->setInitialState(sReady);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_StartUp, sRoughRegulation);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_GotoDesValue, sRoughRegulation)->setTransitionType(QAbstractTransition::InternalTransition);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_ShutDown, sShutDown)->setTransitionType(QAbstractTransition::InternalTransition);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_Interrupted, sHalted)->setTransitionType(QAbstractTransition::InternalTransition);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_Increment , sIncrement)->setTransitionType(QAbstractTransition::InternalTransition);
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_Decrement , sDecrement)->setTransitionType(QAbstractTransition::InternalTransition);
             auto tOffInOn = sOn->addTransition(this, &SourcePowerSupply::SIGNAL_Off, sOff);
             QObject::connect(tOffInOn, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("L'alim coupée de façon inattendue"); });
             sOn->addTransition(this, &SourcePowerSupply::SIGNAL_SwitchOff, sCheckTurnOffAllowed);
             auto tCommandOnInOn = sOn->addTransition(this, &SourcePowerSupply::SIGNAL_SwitchOn, sReady);
             QObject::connect(tCommandOnInOn, &QSignalTransition::triggered, this, [&]() {
                 DebugMessage("La commande de mise en marche lorsque l'alimentation est sous tension est ignorée"); });

                // CheckTurnOffAllowed
                QObject::connect(sCheckTurnOffAllowed, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
                QObject::connect(sCheckTurnOffAllowed, &global::TimedState::entered, this, &SourcePowerSupply::ReadParams);
                sCheckTurnOffAllowed->addTransition(&voltage_, &RegulatedParameter::SIGNAL_Zero, sSwitchOff);
                auto tNotZero = sCheckTurnOffAllowed->addTransition(&voltage_, &RegulatedParameter::SIGNAL_NotZero, sHalted);
                QObject::connect(tNotZero, &QSignalTransition::triggered, this, [&]() {
                    WarningMessage("La commande d'arrêt ne peut être effectuée que lorsque l'alimentation est proche de zéro"); });

                  // SetActAsSetPoint
                 QObject::connect(sSetActAsSetPoint, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
                 QObject::connect(sSetActAsSetPoint, &QState::entered, &tPing_, &QTimer::stop);
                 QObject::connect(sSetActAsSetPoint, &QState::entered, &regulated_, &RegulatedParameter::SetActAsDesValue);
                 sSetActAsSetPoint->addTransition(&regulated_, &RegulatedParameter::SIGNAL_DesValueUpdated, sReady);

                 // Halted
                 QObject::connect(sHalted, &QState::entered, this, [&]() { tPing_.start(3000); });

                 // RoughRegulation
                 QObject::connect(sRoughRegulation, &QState::entered, this, [&]() { tPing_.start(1000); });
                 CreateRoughRegulationStates(sRoughRegulation);
                 sRoughRegulation->addTransition(sRoughRegulation, &QState::finished, sFineRegulation);

                 // FineRegulation
                 QObject::connect(sFineRegulation, &QState::entered, this, [&]() { tPing_.start(1000); });
                 CreateFineRegulationStates(sFineRegulation);
                 auto tOnTarget = sFineRegulation->addTransition(sFineRegulation, &QState::finished, sOnTarget);
                 QObject::connect(tOnTarget, &QSignalTransition::triggered, this, [&]() { InfoMessage("L'alim est démarrée et on target"); });

                 // Increment
                 QObject::connect(sIncrement, &QState::entered, this, [&]() { tPing_.start(500); });
                 QObject::connect(sIncrement, &QState::entered, this, &SourcePowerSupply::IncVoltage);
                 sIncrement->addTransition(this, &SourcePowerSupply::SIGNAL_Interrupted, sReady);

                 // Decrement
                 QObject::connect(sDecrement, &QState::entered, this, [&]() { tPing_.start(500); });
                 QObject::connect(sDecrement, &QState::entered, this, &SourcePowerSupply::DecVoltage);
                 sDecrement->addTransition(this, &SourcePowerSupply::SIGNAL_Interrupted, sReady);

                 // Ready
                 QObject::connect(sReady, &QState::entered, this, [&]() { tPing_.start(2000); });
                 sReady->setInitialState(sCheckOnOffTarget);

                     // CheckOnOffTarget
                     QObject::connect(sCheckOnOffTarget, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
                     sCheckOnOffTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sOnTarget);

                     // OffTarget
                     // QObject::connect(sOffTarget, &QState::entered, this, [&]() { WarningMessage("L'alim est off target"); });
                     QObject::connect(sOffTarget, &QState::entered, this, &SourcePowerSupply::SIGNAL_OffTarget);
                     sOffTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sOnTarget);

                     // OnTarget
                     // QObject::connect(sOnTarget, &QState::entered, this, [&]() { InfoMessage("L'alim est on target"); });
                     QObject::connect(sOnTarget, &QState::entered, this, &SourcePowerSupply::SIGNAL_OnTarget);
                     sOnTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, sOffTarget);

                // ShutDown
                sShutDown->setInitialState(sRampDown);
                QObject::connect(sShutDown, &QState::entered, this, [&]() { DebugMessage("L'alim est en cours d'arrêt"); });
                QObject::connect(sShutDown, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);
                QObject::connect(sShutDown, &QState::entered, this, [&]() { tPing_.start(1000); });
                auto tOffInShutdown = sShutDown->addTransition(this, &SourcePowerSupply::SIGNAL_Off, sOff);
                QObject::connect(tOffInShutdown, &QSignalTransition::triggered, this, [&]() { InfoMessage("L'alim est arrêtée"); });

                    // RampDown
                    QObject::connect(sRampDown, &QState::entered, this, &SourcePowerSupply::DecVoltage);
                    sRampDown->addTransition(&voltage_, &RegulatedParameter::SIGNAL_Zero, sCoolDown);

                    // CoolDown
                    // no more actions

                    // SwitchOff
                    QObject::connect(sSwitchOff, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::DebugMessage);
                    QObject::connect(sSwitchOff, &QState::entered, this, &SourcePowerSupply::WriteOff);

        PrintStateChanges(sSuperState, "SuperState");
        PrintStateChanges(sConnected, "Connected");
        PrintStateChanges(sDisconnected, "Disconnected");
        PrintStateChanges(sCheckMode, "CheckMode");
        PrintStateChanges(sRemote, "Remote");
        PrintStateChanges(sLocal, "Local");
        PrintStateChanges(sADCConnection, "ADCConnection");
        PrintStateChanges(sOmronConnection, "OmronConnection");
        PrintStateChanges(sOff, "Off");
        PrintStateChanges(sIdle, "Idle");
        PrintStateChanges(sCheckState, "CheckState");
        PrintStateChanges(sReadStatus, "ReadStatus");
        PrintStateChanges(sReadValue, "ReadValue");
        PrintStateChanges(sStartUp, "StartUp");
        PrintStateChanges(sOn, "On");
        PrintStateChanges(sSetActAsSetPoint, "SetActAsSetPoint");
        PrintStateChanges(sHalted, "Halted");
        PrintStateChanges(sRoughRegulation, "RoughRegulation");
        PrintStateChanges(sFineRegulation, "FineRegulation");
        PrintStateChanges(sReady, "Ready");
        PrintStateChanges(sCheckOnOffTarget, "CheckOnOffTarget");
        PrintStateChanges(sOffTarget, "OffTarget");
        PrintStateChanges(sOnTarget, "OnTarget");
        PrintStateChanges(sIncrement, "Increment");
        PrintStateChanges(sDecrement, "Decrement");
        PrintStateChanges(sShutDown, "ShutDown");
        PrintStateChanges(sRampDown, "RampDown");
        PrintStateChanges(sSwitchOff, "SwitchOff");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void SourcePowerSupply::CreateStartupStates(global::TimedState *parent) {

    global::TimedState *sVerifyOn = new global::TimedState(parent, 4000, "Echec de lecture de l'état de l'alim");
    global::TimedState *sSwitchOn = new global::TimedState(parent, 3000, "Wait for switching off", sVerifyOn);
    QFinalState *sDone = new QFinalState(parent);
    global::TimedState *sWaitHeatedUp = new global::TimedState(parent, config_.heat_up_time(), "wait", sDone);

    parent->setInitialState(sSwitchOn);

    // SwitchOn
    // the state transitions to verifyon after timeout (see declaration)
    QObject::connect(sSwitchOn, &QState::entered, this, &SourcePowerSupply::WriteOn);

    // VerifyOn
    QObject::connect(sVerifyOn, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
    QObject::connect(sVerifyOn, &QState::entered, this, &SourcePowerSupply::ReadIsOn);
    auto tSwitchedOn = sVerifyOn->addTransition(this, &SourcePowerSupply::SIGNAL_On, sWaitHeatedUp);
    QObject::connect(tSwitchedOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Alimentation sous tension"); });
    auto tOffInStartUp = sVerifyOn->addTransition(this, &SourcePowerSupply::SIGNAL_Off, sSwitchOn);
    QObject::connect(tOffInStartUp, &QSignalTransition::triggered, this, [&]() { WarningMessage("Echec de la mise en marche de l'alim"); });

    // WaitHeatedUp
    QObject::connect(sWaitHeatedUp, &global::TimedState::entered, this, [&]() { DebugMessage("Attente paramétré"); });

    // Done
    // No more actions

    PrintStateChanges(sSwitchOn, "SwitchOn");
    PrintStateChanges(sVerifyOn, "VerifyOn");
    PrintStateChanges(sWaitHeatedUp, "WaitHeatedUp");
    PrintStateChanges(sDone, "Done");
}

void SourcePowerSupply::CreateRoughRegulationStates(global::TimedState *parent) {

    global::TimedState *sEvaluatePosition = new global::TimedState(parent, 3000, "Echec de lecture de la tension de l'alim");
    global::TimedState *sMove = new global::TimedState(parent, 2000, "Wait Move", sEvaluatePosition);
    QFinalState *sDone = new QFinalState(parent);

    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);
    parent->setInitialState(sEvaluatePosition);

    // EvaluatePosition
    QObject::connect(sEvaluatePosition, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
    QObject::connect(sEvaluatePosition, &QState::entered, this, &SourcePowerSupply::ReadRegulatedParam);
    sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_NearTarget, sDone);
    sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_FarFromTarget, sMove);

    // Move
    QObject::connect(sMove, &QState::entered, this, &SourcePowerSupply::DoRoughStep);

    // Done
    // no further actions

    PrintStateChanges(sEvaluatePosition, "EvaluatePosition");
    PrintStateChanges(sMove, "Move");
    PrintStateChanges(sDone, "Done");
}

void SourcePowerSupply::CreateFineRegulationStates(global::TimedState *parent) {

    global::TimedState *sIncreaseTolerance = new global::TimedState(parent, 3000, "Wait");
    global::TimedState *sIteration = new global::TimedState(parent, 15000, "La boucle fine de l'alim ne converge pas, la tolérance est augmentée", sIncreaseTolerance);
        global::TimedState *sEvaluatePosition = new global::TimedState(sIteration, 3000, "Echec de lecture");
        QState *sMoveOneStepCloser = new QState(sIteration);
        global::TimedState *sFillReadBuffer = new global::TimedState(sIteration, 5000, "Wait increment", sEvaluatePosition);
        global::TimedState *sWait = new global::TimedState(sIteration, 3000, "Wait increment", sFillReadBuffer);
    QFinalState *sDone = new QFinalState(parent);

    // parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::SIGNAL_Error);
    QObject::connect(parent, &QState::entered, &regulated_, &RegulatedParameter::SetDefaultTolerance);
    parent->setInitialState(sIteration);

        // Iteration
        QObject::connect(sIteration, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
        sIteration->setInitialState(sEvaluatePosition);

            // EvaluatePosition
            QObject::connect(sEvaluatePosition, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
            QObject::connect(sEvaluatePosition, &QState::entered, this, &SourcePowerSupply::ReadRegulatedParam);
            sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sDone);
            sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, sMoveOneStepCloser);

            // MoveOneStepCloser
            QObject::connect(sMoveOneStepCloser, &QState::entered, this, &SourcePowerSupply::DoFineStep);
            sMoveOneStepCloser->addTransition(sMoveOneStepCloser, &QState::entered, sWait);

            // Wait
            (void)sWait;

            // FillReadBuffer
            QObject::connect(sFillReadBuffer, &global::TimedState::SIGNAL_Timeout, this, &SourcePowerSupply::WarningMessage);
            QObject::connect(sFillReadBuffer, &QState::entered, this, [&]() {
                for (int i = 0; i < config_.regulated_param_config().buffer_size(); ++i) ReadRegulatedParam();
                ReadCurrent(); // just to have a clear signal out
            });
            sFillReadBuffer->addTransition(this, &SourcePowerSupply::SIGNAL_Current, sEvaluatePosition);

        // IncreaseTolerance
        QObject::connect(sIncreaseTolerance, &QState::entered, &regulated_, &RegulatedParameter::DoubleTolerance);
        sIncreaseTolerance->addTransition(sIncreaseTolerance, &global::TimedState::SIGNAL_Timeout, sEvaluatePosition);

        // Done
        // No more actions

    PrintStateChanges(sIteration, "Iteration");
    PrintStateChanges(sIncreaseTolerance, "IncreaseTolerance");
    PrintStateChanges(sEvaluatePosition, "EvaluatePosition");
    PrintStateChanges(sMoveOneStepCloser, "MoveOnStepCloser");
    PrintStateChanges(sFillReadBuffer, "FillReadBuffer");
    PrintStateChanges(sWait, "Wait");
    PrintStateChanges(sDone, "Done");
}

void SourcePowerSupply::ConnectSignals() {
    qDebug() << "SourcePowerSupply::ConnectSignals";
    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, &SourcePowerSupply::InterpretOmronBitRead);
    QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, &SourcePowerSupply::InterpretADCChannelRead);
    QObject::connect(this, &SourcePowerSupply::SIGNAL_Interrupted, this, &SourcePowerSupply::WarningMessage);
    QObject::connect(this, &SourcePowerSupply::SIGNAL_Error, this, &SourcePowerSupply::ErrorMessage);
}

void SourcePowerSupply::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "PowerSupply ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "PowerSupply <-" << name; });
}

void SourcePowerSupply::DoRoughStep() {
    SETPOINTCORR corr = regulated_.GetSetPointCorrection();
    if (corr == SETPOINTCORR::INC) {
        IncVoltage();
    } else if (corr == SETPOINTCORR::DEC) {
        DecVoltage();
    } else {
        qDebug() << "SourcePowerSupply::DoRoughStep No action";
    }
}

void SourcePowerSupply::DoFineStep() {
    SETPOINTCORR corr = regulated_.GetMeanSetPointCorrection();
    if (corr == SETPOINTCORR::INC) {
        IncVoltage();
    } else if (corr == SETPOINTCORR::DEC) {
        DecVoltage();
    } else {
        qDebug() << "SourcePowerSupply::DoFineStep No action";
    }
}

void SourcePowerSupply::InterpretOmronBitRead(const QString& channel, bool content) {
//    qDebug() << "OMRON READ " << channel << " " << content;
    if (channel == (QString("Alim ") + config_.name() + QString(" ON"))) {
        if (content) {
            emit SIGNAL_On();
        } else {
            emit SIGNAL_Off();
        }
    } else if (channel == QString("Alims en distance")) {
        content ? emit SIGNAL_Remote() : emit SIGNAL_Local();
    }
}

void SourcePowerSupply::InterpretADCChannelRead(const QString& channel, double value, QString unit) {
    (void)unit;
//   qDebug() << "ADC READ " << channel << " " << value;
   if (channel == ("V." + config_.name() + " Source")) {
       voltage_.AddActValue(value);
        emit SIGNAL_Voltage(value);
   } else if (channel == ("I." + config_.name() + " Source")) {
       emit SIGNAL_Current(value);
   }
   if (channel == config_.regulated_param()) {
       regulated_.AddActValue(value);
       emit SIGNAL_RegulatedParamValue(value);
   }
}

void SourcePowerSupply::ReadVoltage() {
//    qDebug() << "READ ACT ADC " << actuating_param_ + "." + name_ + " Source";
    adc_.Read("V." + config_.name() + " Source");
}


void SourcePowerSupply::ReadCurrent() {
//    qDebug() << "READ SEC ADC " << secondary_param_ + "." + name_ + " Source";
    adc_.Read("I." + config_.name() + " Source");
}

void SourcePowerSupply::ReadRegulatedParam() {
//    qDebug() << "READ REG " << regulated_param_;
    adc_.Read(config_.regulated_param());
}

void SourcePowerSupply::ReadMode() {
    omron_.ReadChannel(QString("Alims en distance"));
}

void SourcePowerSupply::ReadIsOn() {
//    qDebug() << "READ " << QString("Alim ") + name_ + " ON";
    omron_.ReadChannel(QString("Alim ") + config_.name() + " ON");
}

void SourcePowerSupply::WriteOn() {
//    qDebug() << "WRITE ON " << QString("On ") + name_;
    omron_.WriteChannel(QString("On ") + config_.name(), true);
}

void SourcePowerSupply::WriteOff() {
//    qDebug() << "WRITE OFF " << QString("Off ") + name_;
    omron_.WriteChannel(QString("Off ") + config_.name(), true);
}

void SourcePowerSupply::IncVoltage() {
    qDebug() << "SourcePowerSupply::IncActuatingParam " << regulated_.UpperLimitPassed() << " " << voltage_.UpperLimitPassed();
    if (!regulated_.UpperLimitPassed() && !voltage_.UpperLimitPassed()) {
        omron_.WriteChannel(QString("Inc ") + config_.name(), 1);
    } else {
        emit SIGNAL_Interrupted("Pas autorisé à incrémenter davantage, limite supérieure dépassée");
    }
}

void SourcePowerSupply::DecVoltage() {
//    qDebug() << "SourcePowerSupply::DecActuatingParam";
    if (!regulated_.LowerLimitPassed() && !voltage_.LowerLimitPassed()) {
        omron_.WriteChannel(QString("Dec ") + config_.name(), 1);
    } else {
        emit SIGNAL_Interrupted("Pas autorisé à decrémenter davantage, limite inférieure dépassée.");
    }
}

void SourcePowerSupply::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), config_.name(), message);
}

void SourcePowerSupply::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), config_.name(), message);
}

void SourcePowerSupply::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), config_.name(), message);
}

void SourcePowerSupply::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), config_.name(), message);
}

void SourcePowerSupply::CheckDbus() {
    if (adc_.isValid() && omron_.isValid()) {
        if (!dbus_connected_) {
            DebugMessage("Dbus connecté");
            dbus_connected_ = true;
            emit SIGNAL_DbusOk();
        }
    } else {
        if (dbus_connected_) {
            ErrorMessage("Dbus déconnecté");
            dbus_connected_ = false;
            emit SIGNAL_DbusError();
        }
    }
}

} // namespace
