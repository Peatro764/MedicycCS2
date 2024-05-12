#include "Controller.h"

#include <QObject>
#include <QDateTime>
#include <QSignalTransition>
#include <QDateTime>

#include "DBus.h"
#include "Parameters.h"

//
//   IMPORTANT: The controller class makes signal slot connections for INSingle inheritated classes, where
//   a dynamic bind is required. Since this is not possible using the new signal slot syntax, the old syntax is used.
//   Keep in mind that the old signal slot syntax only checks the validity of this connection on run-time, and emits
//   a debug message.
//

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

Controller::Controller(INSingle* nsingle, int start_delay)
    : nsingle_(nsingle),
      regulated_(nsingle->config()),
      active_compensation_(nsingle->config().active_compensation()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()) {
    qDebug() << "Controller::Controller";
    ConnectSignals();
    SetupTimers();
    QTimer::singleShot(start_delay, this, [&]() { SetupStateMachine(); });
    DebugMessage(nsingle_->name() + " s'est reveillé");
}

Controller::~Controller() {
}

void Controller::ConnectSignals() {
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, this, &Controller::SIGNAL_OffTarget);
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_DesValue, this, &Controller::SIGNAL_DesValue);
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_DesUpdated, this, &Controller::SIGNAL_DesValue);
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_DesUpdated, this, &Controller::DecideRegulationType);
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_ActUpdated, this, &Controller::SIGNAL_ActValue);
    QObject::connect(&regulated_, &RegulatedParameter::SIGNAL_DesValueOutOfBounds, this, &Controller::SIGNAL_DesValueNotAllowed);
    QObject::connect(this, &Controller::SIGNAL_DesValueNotAllowed, this, [&]() { emit SIGNAL_Error("La consigne demandé est hors limites"); });
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_On()), this, SIGNAL(SIGNAL_On())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), this, SIGNAL(SIGNAL_Off())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), &regulated_, SLOT(SetPointReceived(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement))));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1Value(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), &regulated_, SLOT(ActValueReceived(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement))));
    QObject::connect(this, &Controller::SIGNAL_Interrupt, this, &Controller::WarningMessage);
    QObject::connect(this, &Controller::SIGNAL_Error, this, &Controller::ErrorMessage);
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Remote()), this, SLOT(RemoteMode())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Local()), this, SLOT(LocalMode())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Error()), this, SIGNAL(SIGNAL_HardwareError())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Ok()), this, SIGNAL(SIGNAL_HardwareOk())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Connected()), this, SIGNAL(SIGNAL_Connected())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Disconnected()), this, SIGNAL(SIGNAL_Disconnected())));
    assert(QObject::connect(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_IOLoad(double)) , this, SIGNAL(SIGNAL_IOLoad(double))));
}

void Controller::RemoteMode() {
    if (local_mode_) {
        local_mode_ = false;
        InfoMessage("Le nsingle est en mode remote");
    }
}

void Controller::LocalMode() {
    if (!local_mode_) {
        local_mode_ = true;
        emit SIGNAL_Error("Le nsingle est en mode local");
    }
}

void Controller::SetupTimers() {
    tPing_.setInterval(nsingle_->config().read_value_interval());
    tPing_.setSingleShot(false);
    QObject::connect(&tPing_, &QTimer::timeout, this, &Controller::Ping);
}

void Controller::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(nsingle_->name()), message);
}

void Controller::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(nsingle_->name()), message);
}

void Controller::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(nsingle_->name()), message);
}

void Controller::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("NSingle %1").arg(nsingle_->name()), message);
}

void Controller::ReadChannel1() {
    nsingle_->ReadChannel1();
}

void Controller::Ping() {
    nsingle_->Ping();
}

void Controller::ReadMultiChannel1(int number) {
    nsingle_->ReadChannel1(number);
}

void Controller::ReadChannel1SetPoint() {
    nsingle_->ReadChannel1SetPoint();
}

void Controller::WriteChannel1SetPoint(const Measurement& m) {
//    qDebug() << "WriteSetPoint " << m.InterpretedValue() << " " << m.sign();
    nsingle_->WriteChannel1SetPoint(m);
}

void Controller::WriteAndVerifyChannel1SetPoint(const Measurement& m) {
  //  qDebug() << "WriteSetPoint " << m.InterpretedValue() << " " << m.sign();
    nsingle_->WriteAndVerifyChannel1SetPoint(m);
}

void Controller::Configure(double physical_value, bool polarity) {
   qDebug() << "Controller::Configure " << nsingle_->name();
    DebugMessage(QString("Configuration: Polarité ") + (polarity ? "+" : "-") + " Consigné " + QString::number(physical_value));
    if (InRange(physical_value)) {
        startup_parameters_.params_set_ = true;
        startup_parameters_.physical_value_ = physical_value;
        startup_parameters_.polarity_ = polarity;
        emit SIGNAL_Configuration(physical_value, polarity);
    } else {
        emit SIGNAL_DesValueNotAllowed();
    }
}

void Controller::QueryState() {
    nsingle_->IsConnected();
    Measurement m_des = regulated_.GetDesValue();
    Measurement m_act = regulated_.GetActValue();
    emit SIGNAL_DesValue(m_des.InterpretedValue(), m_des.sign());
    emit SIGNAL_ActValue(m_act.InterpretedValue(), m_act.sign());
    if (startup_parameters_.params_set_) {
        emit SIGNAL_Configuration(startup_parameters_.physical_value_, startup_parameters_.polarity_);
    } else {
        emit SIGNAL_Error("L'alimentation n'a pas été configurée");
    }
}

void Controller::Startup() {
    qDebug() << "Controller::Startup()";
    if (startup_parameters_.params_set_) {
        emit SIGNAL_Startup();
    } else {
        emit SIGNAL_Error("L'alimentation n'a pas été configurée");
    }
}

void Controller::UpdateDesiredValue(double physical_value) {
    qDebug() << "Controller::UpdateDesiredValue " << physical_value;
    if (InRange(physical_value)) {
        regulated_.SetDesPhysicalValue(physical_value);
    } else {
        emit SIGNAL_DesValueNotAllowed();
    }
}

void Controller::ChangePolarity() {
    qDebug() << "Controller::ChangePolarity";
    if (nsingle_->config().polarity_change_allowed()) {
        emit SIGNAL_ChangePolarity();
    } else {
        WarningMessage("Changement de polarité non autorisé pour cette alimentation");
    }
}

void Controller::SetActiveCompensation(bool on) {
    active_compensation_ = on;
    emit SIGNAL_ActiveCompensation(active_compensation_);
}

void Controller::DecideRegulationType() {
   if (std::abs(regulated_.DesAndActValueDifference()) > 7 * nsingle_->config().GetChannel1ValueConfig().min()) {
       emit SIGNAL_RoughAdjust();
   } else {
       emit SIGNAL_FineAdjust();
   }
}

void Controller::EmitSignalsToUI() {
    emit SIGNAL_ActiveCompensation(active_compensation_);
}

bool Controller::InRange(double physical_value) const {
    return (std::abs(physical_value) <= nsingle_->config().upper_limit() &&
            physical_value >= 0.0);
}

void Controller::Shutdown() {
    qDebug() << "Controller::Shutdown";
    emit SIGNAL_Shutdown();
}

void Controller::Interrupt() {
    qDebug() << "Controller::Interrupt";
    emit SIGNAL_Interrupt("L'alimentation a été interrompue par l'opérateur");
}

void Controller::Increment(int nbits) {
    qDebug() << "Controller::Increment " << nbits;
    if (!InRange(regulated_.GetActValue().InterpretedValue())) {
        emit SIGNAL_DesValueNotAllowed();
        return;
    }
    // Due to the fluctions of the BP read value, a regulation loop can not be applied
    // in this case, but just raw steps in set and des values
    if (nsingle_->config().name() == QString("Bobine_Principale")) {
        regulated_.IncrementSetAndDesValue(nbits);
        emit SIGNAL_StepAdjust();
    } else {
        regulated_.IncrementDesValue(nbits);
    }
}

void Controller::Decrement(int nbits) {
    qDebug() << "Controller::Decrement " << nbits;
    // Due to the fluctions of the BP read value, a regulation loop can not be applied
    // in this case, but just raw steps in set and des values
    if (nsingle_->config().name() == QString("Bobine_Principale")) {
        regulated_.DecrementDesValue(nbits);
        emit SIGNAL_StepAdjust();
    } else {
        regulated_.DecrementDesValue(nbits);
    }
}

void Controller::Reset() {
    nsingle_->Reset();
}

void Controller::On() {
    nsingle_->On();
}

void Controller::Off() {
    nsingle_->Off();
}

void Controller::ReadState() {
    nsingle_->ReadState();
}

void Controller::SetupStateMachine() {
    qDebug() << "Controller::SetupStateMachine";    

    // State Declarations

    QState *sSuperState = new QState();
        QState *sDisconnected = new QState(sSuperState);
        QState *sConnected = new QState(sSuperState);
           QState *sOff = new QState(sConnected);
           global::TimedState *sChangePolarity = new global::TimedState(sConnected, 20000, "Echec du changement de polarité", sOff);
           global::TimedState *sCheckState = new global::TimedState(sConnected, nsingle_->config().timeout_checkstate() , "Echec de la vérification de l'état actuel", sOff);
           global::TimedState *sStartup = new global::TimedState(sConnected, nsingle_->config().timeout_startup(), "La procédure de démarrage a duré plus longtemps que autorisée", sOff);
           QState *sOn = new QState(sConnected);
               QState *sHalted = new QState(sOn);
               global::TimedState *sCheckActValue = new global::TimedState(sOn, 15000 * nsingle_->config().timeout_command_factor(), "Échec de la lecture de la valeur actuelle", sHalted);
               global::TimedState *sRoughRegulation = new global::TimedState(sOn, nsingle_->config().timeout_roughpositioning(), "La régulation approximative a échoué", sHalted);
               global::TimedState *sFineRegulation = new global::TimedState(sOn, nsingle_->config().timeout_finepositioning(), "La régulation fine a échoué", sHalted);
               global::TimedState *sStepAdjust = new global::TimedState(sOn, 15000, "La procédure d'incrémentation de la consigné a échoué", sHalted);
               QState *sReady = new QState(sOn);
               global::TimedState *sShutdown = new global::TimedState(sOn, nsingle_->config().timeout_shutdown(), "Echec de l'arrêt de l'alimentation", sHalted);

    // Print state changes
    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sDisconnected, "Disconnected");
    PrintStateChanges(sConnected, "Connected");
    PrintStateChanges(sOff, "Off");
    PrintStateChanges(sCheckState, "CheckState");
    PrintStateChanges(sStartup, "Startup");
    PrintStateChanges(sOn, "On");
    PrintStateChanges(sCheckActValue, "CheckActValue");
    PrintStateChanges(sHalted, "Halted");
    PrintStateChanges(sRoughRegulation, "RoughRegulation");
    PrintStateChanges(sFineRegulation, "FineRegulation");
    PrintStateChanges(sStepAdjust, "StepAdjust");
    PrintStateChanges(sReady, "Ready");
    PrintStateChanges(sShutdown, "Shutdown");
    PrintStateChanges(sChangePolarity, "ChangePolarity");

    // State Definitions

    // SuperState
    sSuperState->setInitialState(sDisconnected);

        // Disconnected
        QObject::connect(sDisconnected, &QState::entered, this, [&]() { nsingle_->IsConnected(); });
        QObject::connect(sDisconnected, &QState::entered, this, [&]() { WarningMessage("Essayer d'établir une connexion.."); });
        QObject::connect(sDisconnected, &QState::entered, &tPing_, &QTimer::stop);
        sDisconnected->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Connected()), sConnected);
        QSignalTransition *tStartUpInDisconnected = new QSignalTransition(this, &Controller::SIGNAL_Startup);
        QSignalTransition *tShutdownInDisconnected = new QSignalTransition(this, &Controller::SIGNAL_Shutdown);
        QSignalTransition *tInterruptInDisconnected = new QSignalTransition(this, &Controller::SIGNAL_Interrupt);
        QSignalTransition *tChangePolarityInDisconnected = new QSignalTransition(this, &Controller::SIGNAL_ChangePolarity);
        sDisconnected->addTransition(tStartUpInDisconnected);
        sDisconnected->addTransition(tShutdownInDisconnected);
        sDisconnected->addTransition(tInterruptInDisconnected);
        sDisconnected->addTransition(tChangePolarityInDisconnected);
        QObject::connect(tStartUpInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Pas de connexion à nsingle"); });
        QObject::connect(tShutdownInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Pas de connexion à nsingle"); });
        QObject::connect(tInterruptInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Pas de connexion à nsingle"); });
        QObject::connect(tChangePolarityInDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Pas de connexion à nsingle"); });

        // Connected
        sConnected->setInitialState(sCheckState);
        QObject::connect(sConnected, &QState::entered, this, [&]() { InfoMessage("Connexion établie"); });
        auto tDisconnected = sConnected->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Disconnected()), sDisconnected);
        QObject::connect(tDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Perte de connexion"); });
        sConnected->addTransition(this, &Controller::SIGNAL_Startup, sStartup)->setTransitionType(QAbstractTransition::InternalTransition);

            // Off
            QObject::connect(sOff, &QState::entered, this, [&](){
                // Multichannel power supply (steerer) cannot read values in off state
                if (nsingle_->config().multichannel()) tPing_.stop(); else tPing_.start();
                emit SIGNAL_TransitionToOff();
            });
            sOff->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_On()), sOn);
            sOff->addTransition(this, &Controller::SIGNAL_ChangePolarity, sChangePolarity);

            // ChangePolarity
            QObject::connect(sChangePolarity, &QState::entered, this, [&](){  tPing_.stop(); });
            CreateChangePolarityStates(sChangePolarity);
            sChangePolarity->addTransition(sChangePolarity, &QState::finished, sOff);

            // CheckState
            QObject::connect(sCheckState, &QState::entered, this, [&](){  tPing_.stop(); });
            CreateCheckStateStates(sCheckState, sOn, sOff);

            // Startup
            QObject::connect(sStartup, &QState::entered, this, [&](){  tPing_.stop(); });
            CreateStartupStates(sStartup);
            sStartup->addTransition(sStartup, &QState::finished, sRoughRegulation);
            sStartup->addTransition(this, &Controller::SIGNAL_Interrupt, sOff);

            // On
            sOn->setInitialState(sCheckActValue);
            QState::connect(sOn, &QState::entered, this, [&]() { emit SIGNAL_TransitionToOn(); });
            sOn->addTransition(this, &Controller::SIGNAL_Shutdown, sShutdown)->setTransitionType(QAbstractTransition::InternalTransition);
            sOn->addTransition(this, &Controller::SIGNAL_Interrupt, sHalted)->setTransitionType(QAbstractTransition::InternalTransition);
            auto tErrorInOn = sOn->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Error()), sOff);
            QObject::connect(tErrorInOn, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("L'alimentation a un défaut"); });
            auto tOffinOn = sOn->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), sOff);
            QObject::connect(tOffinOn, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error("Alimentation coupée de façon inattendue"); });

               // CheckActValue
               QObject::connect(sCheckActValue, &QState::entered, this, [&](){  tPing_.stop(); });
               CreateCheckActValueStates(sCheckActValue, sReady, sHalted);

               // Halted
               QObject::connect(sHalted, &QState::entered, this, [&](){  tPing_.start(); });
               // Special case: steerers are turned on/off in group by using the multiplexed_2 nsingle. This  line avoids triggering the error; Alim coupée de facon inattandu
               if (nsingle_->name().contains("Steerer_")) { sHalted->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), sOff); }
               sHalted->addTransition(this, &Controller::SIGNAL_RoughAdjust, sRoughRegulation);
               sHalted->addTransition(this, &Controller::SIGNAL_FineAdjust, sFineRegulation);
               sHalted->addTransition(this, &Controller::SIGNAL_StepAdjust, sStepAdjust);

               // RoughRegulation
               QObject::connect(sRoughRegulation, &QState::entered, this, [&](){  tPing_.stop(); });
               CreateRoughRegulationStates(sRoughRegulation);
               sRoughRegulation->addTransition(sRoughRegulation, &QState::finished, sFineRegulation);
               sRoughRegulation->addTransition(this, &Controller::SIGNAL_RoughAdjust, sRoughRegulation);
               sRoughRegulation->addTransition(this, &Controller::SIGNAL_FineAdjust, sFineRegulation);
               sRoughRegulation->addTransition(this, &Controller::SIGNAL_StepAdjust, sStepAdjust);

               // FineRegulation
               QObject::connect(sFineRegulation, &QState::entered, this, [&](){  tPing_.stop(); });
               CreateFineRegulationStates(sFineRegulation);
               sFineRegulation->addTransition(sFineRegulation, &QState::finished, sReady);
               sFineRegulation->addTransition(this, &Controller::SIGNAL_RoughAdjust, sRoughRegulation);
               sFineRegulation->addTransition(this, &Controller::SIGNAL_StepAdjust, sStepAdjust);

                // StepAdjust
               QObject::connect(sStepAdjust, &QState::entered, this, [&](){  tPing_.stop(); });
               CreateStepAdjustStates(sStepAdjust);
               sStepAdjust->addTransition(sStepAdjust, &QState::finished, sReady);
               sStepAdjust->addTransition(this, &Controller::SIGNAL_RoughAdjust, sRoughRegulation);
               sStepAdjust->addTransition(this, &Controller::SIGNAL_FineAdjust, sFineRegulation);
               sStepAdjust->addTransition(this, &Controller::SIGNAL_StepAdjust, sStepAdjust);

               // Ready
               QObject::connect(sReady, &QState::entered, this, [&](){  tPing_.start(); });
               CreateReadyStates(sReady);
               sReady->addTransition(this, &Controller::SIGNAL_RoughAdjust, sRoughRegulation);
               sReady->addTransition(this, &Controller::SIGNAL_FineAdjust, sFineRegulation);
               sReady->addTransition(this, &Controller::SIGNAL_StepAdjust, sStepAdjust);

               // Shutdown
               QObject::connect(sShutdown, &QState::entered, this, [&](){  tPing_.stop(); });
               CreateShutdownStates(sShutdown);
               sShutdown->addTransition(sShutdown, &QState::finished, nsingle_->config().switchoff_on_shutdown() ? sOff : sHalted);

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();

    qDebug() << "Controller::SetupStateMachine Done";
}

void Controller::CreateCheckStateStates(global::TimedState *parent, QState *sOn, QState *sOff) {
    global::TimedState *sReadState = new global::TimedState(parent, 2000 * nsingle_->config().timeout_command_factor(), "Echec de lecture de l'état actuel");
    global::TimedState *sCheckDesValueParent = new global::TimedState(parent, 5000 * nsingle_->config().timeout_command_factor(), "Abandon, passage à l'étape suivante", sReadState);
        global::TimedState *sCheckDesValue = new global::TimedState(sCheckDesValueParent, 2000 * nsingle_->config().timeout_command_factor(), "Echec de la verification de la consigne");

    PrintStateChanges(sCheckDesValueParent, "CheckDesValueParent");
    PrintStateChanges(sCheckDesValue, "CheckDesValue");
    PrintStateChanges(sReadState, "ReadState");

    // Parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    QObject::connect(parent, &global::TimedState::entered, this, &Controller::EmitSignalsToUI);
    parent->setInitialState(sCheckDesValueParent);

    // CheckDesValueParent
    QObject::connect(sCheckDesValueParent, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    sCheckDesValueParent->setInitialState(sCheckDesValue);

    // CheckDesValue
    QObject::connect(sCheckDesValue, &global::TimedState::SIGNAL_Timeout, this, &Controller::DebugMessage);
    QObject::connect(sCheckDesValue, &QState::entered, this, [&]() {
        if (startup_parameters_.params_set_) {
	    qDebug() << "Controller " << nsingle_->name() << " already configured, reading channel1 setpoint";
            // This probably means that there was a temporary connection break with the nsingle since a configuration exists.
            // Read setpoint in nsingle and check that it compares to the one stored in the controller.
            ReadChannel1SetPoint();
        } else {
            // This probably means that the nsingle controller process was restarted, since it doesnt have a configuration
            // Ask middlelayer for a new configuration
	    qDebug() << "Controller " << nsingle_->name() << " DemandForConfiguration";
            emit SIGNAL_DemandForConfiguration();
        }
    });
    sCheckDesValue->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsMatch, sReadState);
    sCheckDesValue->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsDontMatch, sReadState);
    auto tConfig = sCheckDesValue->addTransition(this, &Controller::SIGNAL_Configuration, sReadState);
    QObject::connect(tConfig, &QSignalTransition::triggered, this, [&]() {
        DebugMessage("Configuration reçue");
        regulated_.SetDesPhysicalValue(startup_parameters_.physical_value_, startup_parameters_.polarity_);
    });

    // ReadState
    QObject::connect(sReadState, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sReadState, &QState::entered, this, &Controller::ReadState);
    sReadState->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), sOff);
    sReadState->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_On()), sOn);
}

void Controller::CreateCheckActValueStates(global::TimedState *parent, QState *sOnTarget, QState *sOffTarget) {

    global::TimedState *sCheckOnTarget = new global::TimedState(parent, 3000 * nsingle_->config().timeout_command_factor(), "Echec de lecture de la valeur actuelle");
    global::TimedState *sReadActValue = new global::TimedState(parent, 8000 * nsingle_->config().timeout_command_factor(), "Wait", sCheckOnTarget);

    PrintStateChanges(sReadActValue, "ReadActValue");
    PrintStateChanges(sCheckOnTarget, "CheckOnTarget");

    // Parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    parent->setInitialState(sReadActValue);

      // ReadActValue
      QObject::connect(sReadActValue, &QState::entered, this, [&]() { ReadMultiChannel1(5); });

      // CheckOnTarget
      QObject::connect(sCheckOnTarget, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
      QObject::connect(sCheckOnTarget, &QState::entered, this, &Controller::ReadChannel1);
      sCheckOnTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sOnTarget);
      sCheckOnTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, sOffTarget);

}

void Controller::CreateStartupStates(global::TimedState *parent) {

    // state definitions
    global::TimedState *sCheckOnOff = new global::TimedState(parent, 3000 * nsingle_->config().timeout_command_factor(), "Echec de la vérification de l'état actuel");
    global::TimedState *sCheckPolarity = new global::TimedState(parent, 3000 * nsingle_->config().timeout_command_factor(), "Echec de la vérification de la polarité de l'alimentation");
    global::TimedState *sShutdown = new global::TimedState(parent, nsingle_->config().timeout_shutdown(), "Echec de l'arrêt de l'alimentation");
    global::TimedState *sReset = new global::TimedState(parent, 5000 * nsingle_->config().timeout_command_factor(), "Echec de la réinitialisation de l'erreur");
    global::TimedState *sPowerUp = new global::TimedState(parent, 15000 * nsingle_->config().timeout_command_factor(), "La procédure de mise sous tension a échoué");
        global::TimedState *sSwitchOn = new global::TimedState(sPowerUp, 6000 * nsingle_->config().timeout_command_factor(), "Echec de la mise sous tension de l'alimentation");
        global::TimedState *sWriteZeroSetPoint = new global::TimedState(sPowerUp, 4000 * nsingle_->config().timeout_command_factor(), "Echec de commande courant zéro");
        global::TimedState *sCheckActState = new global::TimedState(sPowerUp, 3000, "Le contrôleur attend une réponse du nsingle..");
    QFinalState *sDone = new QFinalState(parent);
    global::TimedState *sWaitOn = new global::TimedState(parent, 5000, "Délai de mise sous tension", sDone);

    // print state changes
    PrintStateChanges(sCheckOnOff, "CheckOnOff");
    PrintStateChanges(sCheckPolarity, "CheckPolarity");
    PrintStateChanges(sReset, "Reset");
    PrintStateChanges(sPowerUp, "PowerUp");
    PrintStateChanges(sSwitchOn, "SwitchOn");
    PrintStateChanges(sWriteZeroSetPoint, "WriteZeroSetPoint");
    PrintStateChanges(sCheckActState, "CheckActState");
    PrintStateChanges(sWaitOn, "WaitOn");
    PrintStateChanges(sDone, "Done");

    // state implementations

    // parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    QObject::connect(parent, &QState::entered, this, [&](){  DebugMessage("L'alimentation est en cours de démarrage"); });
    QObject::connect(parent, &QState::entered, this, [&](){ regulated_.SetDesPhysicalValue(startup_parameters_.physical_value_, startup_parameters_.polarity_); });
    parent->setInitialState(sCheckOnOff);   

    // CheckOnOff
    QObject::connect(sCheckOnOff, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sCheckOnOff, &QState::entered, this, &Controller::ReadState);
    sCheckOnOff->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_On()), nsingle_->config().hot_polarity_change() ? sWaitOn : sCheckPolarity);
    sCheckOnOff->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), nsingle_->config().reset_at_on() ? sPowerUp : sReset);

    // CheckPolarity
    QObject::connect(sCheckPolarity, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sCheckPolarity, &QState::entered, this, &Controller::ReadChannel1);
    sCheckPolarity->addTransition(&regulated_, &RegulatedParameter::SIGNAL_PolarityCorrect, sWaitOn);
    auto tPolarityWrong = sCheckPolarity->addTransition(&regulated_, &RegulatedParameter::SIGNAL_PolarityWrong, sShutdown);
    QObject::connect(tPolarityWrong, &QSignalTransition::triggered, this, [&]() { WarningMessage("Changement de polarité requis. Arrêt de l'alimentation"); });

    // Shutdown
    CreateShutdownStates(sShutdown);
    sShutdown->addTransition(sShutdown, &QState::finished, parent);

    // Reset
    QObject::connect(sReset, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sReset, &QState::entered, this, &Controller::Reset); // Note that the Reset command includes a ReadState.
    sReset->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Ok()), sPowerUp);

    // PowerUp
    QObject::connect(sPowerUp, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    sPowerUp->setInitialState(sWriteZeroSetPoint);
    sPowerUp->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_On()), sWaitOn);

        // WriteZeroSetPoint. Note that this can change the polarity of the power supply.
        QObject::connect(sWriteZeroSetPoint, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
        QObject::connect(sWriteZeroSetPoint, &QState::entered, this, [&]() {
            regulated_.SetDesPhysicalValue(0, regulated_.GetDesValue().sign());
            WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint()); });
        if (nsingle_->config().verify_setpoints()) { sWriteZeroSetPoint->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsMatch, sCheckActState); }
        else { sWriteZeroSetPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sSwitchOn); }

        // CheckActState
        QObject::connect(sCheckActState, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
        QObject::connect(sCheckActState, &QState::entered, this, &Controller::ReadChannel1);
        sCheckActState->addTransition(&regulated_, &RegulatedParameter::SIGNAL_PolarityCorrect, sSwitchOn);
        auto tPolarityWrongInWaitReady = sCheckActState->addTransition(&regulated_, &RegulatedParameter::SIGNAL_PolarityWrong, sCheckOnOff);
        QObject::connect(tPolarityWrongInWaitReady, &QSignalTransition::triggered, this, [&]() { WarningMessage("Changement de polarité pas encore fait"); });

        // SwitchOn
        QObject::connect(sSwitchOn, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
        QObject::connect(sSwitchOn, &QState::entered, this, &Controller::On);

    // WaitOn
    // To allow for the power supply to properly power up. Transits automatically after delay
    QObject::connect(sWaitOn, &QState::entered, this, [&](){ regulated_.SetDesPhysicalValue(startup_parameters_.physical_value_, startup_parameters_.polarity_); });
    QObject::connect(sWaitOn, &QState::entered, this, [&]() { DebugMessage("l'alimentation a été mise sous tension"); });

    // Done
    QObject::connect(sDone, &QFinalState::entered, this, &Controller::SIGNAL_Done);
}

void Controller::CreateRoughRegulationStates(global::TimedState *parent) {

    QState *sRamp = new QState(parent);
        QState *sExecuteRampPoint = new QState(sRamp);
        global::TimedState *sIncrementRampPoint = new global::TimedState(sRamp, 1000 * nsingle_->config().ramp_cmd_inc_time(), "WaitRamp", sExecuteRampPoint);
    global::TimedState *sWriteFinalSetPoint = new global::TimedState(parent, 4000 * nsingle_->config().timeout_command_factor(), "Echec d'écriture de consigne");
    QState *sWaitUntilStable = new QState(parent);
        global::TimedState *sWaitUntilNearTarget = new global::TimedState(sWaitUntilStable, 1000, "WaitUntilNearTarget");
        QState *sIncreaseTolerance = new QState(sWaitUntilStable);
        global::TimedState *sCheckSignalStableParent = new global::TimedState(sWaitUntilStable, 12000, "La régulation brut ne converge pas, la tolérance est augmentée", sIncreaseTolerance);
            global::TimedState *sCheckSignalStable = new global::TimedState(sCheckSignalStableParent, 1000, "CheckSignalStable");
    QFinalState *sDone = new QFinalState(parent);

    // print state changes
    PrintStateChanges(sRamp, "Ramp");
        PrintStateChanges(sExecuteRampPoint, "ExecuteRampPoint");
        PrintStateChanges(sIncrementRampPoint, "IncrementRampPoint");
    PrintStateChanges(sWriteFinalSetPoint, "WriteFinalSetPoint");
    PrintStateChanges(sWaitUntilStable, "WaitUntilStable");
        PrintStateChanges(sWaitUntilNearTarget, "WaitUntilNearTarget");
        PrintStateChanges(sCheckSignalStableParent, "CheckSignalStableParent");
        PrintStateChanges(sCheckSignalStable, "CheckSignalStable");
        PrintStateChanges(sIncreaseTolerance, "IncreaseTolerance");
        PrintStateChanges(sDone, "Done");

    // Parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    if (nsingle_->config().ramp_activated()) {
        parent->setInitialState(sRamp);
    } else {
        parent->setInitialState(sWriteFinalSetPoint);
    }

    // Ramp
    QObject::connect(sRamp, &QState::entered, &regulated_, &RegulatedParameter::SetRampParameters);
    auto tWriteFinalSetPoint = sRamp->addTransition(&regulated_, &RegulatedParameter::SIGNAL_RampFinished, sWriteFinalSetPoint);
    tWriteFinalSetPoint->setTransitionType(QAbstractTransition::InternalTransition);
    sRamp->setInitialState(sExecuteRampPoint);

        // ExecuteRampPoint
        QObject::connect(sExecuteRampPoint, &QState::entered, this, [&]() {
            regulated_.SetRampSetPoint();
            WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint()); });
        if (nsingle_->config().verify_setpoints()) {
            auto tIterateRamp = sExecuteRampPoint->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsMatch, sIncrementRampPoint);
            tIterateRamp->setTransitionType(QAbstractTransition::InternalTransition);
        } else {
            auto tIterateRamp = sExecuteRampPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sIncrementRampPoint);
            tIterateRamp->setTransitionType(QAbstractTransition::InternalTransition);
        }

        // IncrementRampPoint
        QObject::connect(sIncrementRampPoint, &QState::entered, this, [&]() {
            ReadChannel1();
            regulated_.IncRampSetPoint();
        });

    // WriteFinalSetPoint
    QObject::connect(sWriteFinalSetPoint, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sWriteFinalSetPoint, &QState::entered, this, [&]() {
        regulated_.SetPointToDesPoint();
        WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint()); });
        if (nsingle_->config().verify_setpoints()) { sWriteFinalSetPoint->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsMatch, sWaitUntilStable); }
        else { sWriteFinalSetPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sWaitUntilStable); }

    // WaitUntilStable
    sWaitUntilStable->setInitialState(sWaitUntilNearTarget);
    QObject::connect(sWaitUntilStable, &QState::entered, &regulated_, &RegulatedParameter::SetDefaultTolerance);

        // WaitUntilNearTarget
        QObject::connect(sWaitUntilNearTarget, &QState::entered, this, &Controller::ReadChannel1);
        auto tWaitUntilNearTarget = sWaitUntilNearTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_NearTarget, sCheckSignalStableParent);
        tWaitUntilNearTarget->setTransitionType(QAbstractTransition::InternalTransition);

        // CheckSignalStableParent
        QObject::connect(sCheckSignalStableParent, &global::TimedState::SIGNAL_Timeout, this, &Controller::DebugMessage);
        sCheckSignalStableParent->setInitialState(sCheckSignalStable);

            // CheckSignalStable
            QObject::connect(sCheckSignalStable, &QState::entered, this, &Controller::ReadChannel1);
            sCheckSignalStable->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SignalSteady, sDone);

        // IncreaseTolerance
        QObject::connect(sIncreaseTolerance, &QState::entered, &regulated_, &RegulatedParameter::SetDoubleTolerance);
        sIncreaseTolerance->addTransition(sIncreaseTolerance, &QState::entered, sCheckSignalStableParent);
 }

void Controller::CreateFineRegulationStates(global::TimedState *parent) {

    QState *sIncreaseTolerance = new QState(parent);
    global::TimedState *sIteration = new global::TimedState(parent, 15000, "La regulation fine ne converge pas, la tolérance est augmentée", sIncreaseTolerance);
        global::TimedState *sEvaluatePosition = new global::TimedState(sIteration, 3000 * nsingle_->config().timeout_command_factor(), "Echec de lecture du courant");
        global::TimedState *sMoveOneBitCloser = new global::TimedState(sIteration, 4000 * nsingle_->config().timeout_command_factor(), "Echec d'écriture du point de consigne");
        global::TimedState *sFillReadBuffer = new global::TimedState(sIteration, 600 * nsingle_->config().regulation_buffer_size() * nsingle_->config().timeout_command_factor(), "Wait increment", sEvaluatePosition);
        global::TimedState *sWait = new global::TimedState(sIteration, 300 * nsingle_->config().timeout_command_factor(), "Wait increment", sFillReadBuffer);
    QFinalState *sDone = new QFinalState(parent);

    // print state changes
    PrintStateChanges(sIteration, "Iteration");
    PrintStateChanges(sIncreaseTolerance, "IncreaseTolerance");
    PrintStateChanges(sEvaluatePosition, "EvaluatePosition");
    PrintStateChanges(sMoveOneBitCloser, "MoveOnBitCloser");
    PrintStateChanges(sFillReadBuffer, "FillReadBuffer");
    PrintStateChanges(sWait, "Wait");
    PrintStateChanges(sDone, "Done");

    // parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    QObject::connect(parent, &QState::entered, &regulated_, &RegulatedParameter::SetDefaultTolerance);
    parent->setInitialState(sIteration);
    parent->addTransition(this, &Controller::SIGNAL_FineAdjust, sWait);

        // Iteration
        QObject::connect(sIteration, &global::TimedState::SIGNAL_Timeout, this, &Controller::DebugMessage);
        sIteration->setInitialState(sEvaluatePosition);

            // EvaluatePosition
            QObject::connect(sEvaluatePosition, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
            QObject::connect(sEvaluatePosition, &QState::entered, this, &Controller::ReadChannel1);
            sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sDone);
            sEvaluatePosition->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, sMoveOneBitCloser);

            // MoveOneBitCloser
            QObject::connect(sMoveOneBitCloser, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
            QObject::connect(sMoveOneBitCloser, &QState::entered, this, [&]() {
                regulated_.SetImprovedSetPoint();
                WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint()); });
            sMoveOneBitCloser->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sWait);

            // Wait
            (void)sWait;

            // FillReadBuffer
            QObject::connect(sFillReadBuffer, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
            QObject::connect(sFillReadBuffer, &QState::entered, this, [&]() {
                ReadMultiChannel1(nsingle_->config().regulation_buffer_size());
                ReadChannel1SetPoint(); // This is done just to have a clear exit signal
            });
            sFillReadBuffer->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sEvaluatePosition);

        // IncreaseTolerance
        QObject::connect(sIncreaseTolerance, &QState::entered, &regulated_, &RegulatedParameter::SetDoubleTolerance);
        sIncreaseTolerance->addTransition(sIncreaseTolerance, &QState::entered, sIteration);
}

void Controller::CreateStepAdjustStates(global::TimedState *parent) {

    global::TimedState *sWriteSetPoint = new global::TimedState(parent, 1000 * nsingle_->config().timeout_command_factor(), "Echec d'écriture du point de consigne");
    QFinalState *sDone = new QFinalState(parent);
    global::TimedState *sReadActValue = new global::TimedState(parent, 10000, "Done reading", sDone);

    // print state changes
    PrintStateChanges(sWriteSetPoint, "WriteSetPoint");
    PrintStateChanges(sReadActValue, "ReadActValue");
    PrintStateChanges(sDone, "Done");

    // parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    parent->setInitialState(sWriteSetPoint);

    // WriteSetPoint
    QObject::connect(sWriteSetPoint, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sWriteSetPoint, &QState::entered, this, [&]() { WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint()); });
    if (nsingle_->config().multichannel() || nsingle_->config().multiplexed()) {
        sWriteSetPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sReadActValue);
    } else {
        sWriteSetPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sDone);
    }
    // ReadActValues
    QTimer* timer_read = new QTimer(sReadActValue);
    timer_read->setSingleShot(false);
    timer_read->setInterval(1800);
    QObject::connect(timer_read, &QTimer::timeout, this, &Controller::ReadChannel1);
    QObject::connect(sReadActValue, &QState::entered, timer_read, qOverload<>(&QTimer::start));
    QObject::connect(sReadActValue, &QState::exited, timer_read, &QTimer::stop);
}


void Controller::CreateReadyStates(QState *parent) {

    QState *sOnTarget = new QState(parent);
    QState *sOffTarget = new QState(parent);

    // print state changes
    PrintStateChanges(sOnTarget, "OnTarget");
    PrintStateChanges(sOffTarget, "OffTarget");

    // parent
    parent->setInitialState(sOnTarget);
    QObject::connect(parent, &QState::entered, &regulated_, &RegulatedParameter::SetReadyTolerance);

    // OnTarget
    QObject::connect(sOnTarget, &QState::entered, this, [&]() { DebugMessage("On target"); });
    sOnTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OffTarget, sOffTarget);
    QSignalTransition *tOnTargetSelf = new QSignalTransition(this, &Controller::SIGNAL_ActValue);
    sOnTarget->addTransition(tOnTargetSelf);
    QObject::connect(tOnTargetSelf, &QAbstractTransition::triggered, this, &Controller::SIGNAL_OnTarget);

    // OffTarget
    // QObject::connect(sOffTarget, &QState::entered, this, [&]() { WarningMessage("Off target"); });
    QObject::connect(sOffTarget, &QState::entered, this, [&]() { if (active_compensation_) DecideRegulationType(); });
    sOffTarget->addTransition(&regulated_, &RegulatedParameter::SIGNAL_OnTarget, sOnTarget);
    sOffTarget->addTransition(this, &Controller::SIGNAL_ActiveCompensation, sOffTarget);
    QSignalTransition *tOffTargetSelf = new QSignalTransition(this, &Controller::SIGNAL_ActValue);
    sOffTarget->addTransition(tOffTargetSelf);
    QObject::connect(tOffTargetSelf, &QAbstractTransition::triggered, this, &Controller::SIGNAL_OffTarget);
}

void Controller::CreateShutdownStates(global::TimedState *parent) {

    global::TimedState *sRampDown = new global::TimedState(parent, nsingle_->config().timeout_roughpositioning(), "RampDown wait");
    global::TimedState *sSwitchOff = new global::TimedState(parent, 5000 * nsingle_->config().timeout_command_factor(), "SwitchOff wait");
    QFinalState *sDone = new QFinalState(parent);

    // print state changes
    PrintStateChanges(sRampDown, "RampDown");
    PrintStateChanges(sSwitchOff, "SwitchOff");
    PrintStateChanges(sDone, "Done");

    // parent
    QObject::connect(parent, &QState::entered, this, [&]() {
        regulated_.SetZeroDesValueWithCurrentPolarity();
        DebugMessage("Le shutdown de l'alimentation a commencé");
    });
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    auto tOff = parent->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Off()), sDone);
    tOff->setTransitionType(QAbstractTransition::InternalTransition);
    QObject::connect(tOff, &QSignalTransition::triggered, this, [&]() { DebugMessage("L'alimentation a été coupée"); });
    parent->setInitialState(sRampDown);

        // RampDown
        QObject::connect(sRampDown, &QState::entered, this, [&](){  tPing_.stop(); });
        CreateRoughRegulationStates(sRampDown);
        if (nsingle_->config().switchoff_on_shutdown()) {
            sRampDown->addTransition(sRampDown, &QState::finished, sSwitchOff);
        } else {
            sRampDown->addTransition(sRampDown, &QState::finished, sDone);
        }

        // SwitchOff
        QObject::connect(sSwitchOff, &QState::entered, this, &Controller::Off);

        // Done
        QObject::connect(sDone, &QState::entered, this, &Controller::SIGNAL_Done);
}

void Controller::CreateChangePolarityStates(global::TimedState *parent) {

    global::TimedState *sReadCurrentPolarity = new global::TimedState(parent, 4000 * nsingle_->config().timeout_command_factor(), "Echec de commande lecture polarité");
    global::TimedState *sWriteZeroSetPoint = new global::TimedState(parent, 4000 * nsingle_->config().timeout_command_factor(), "Echec de commande consigne courant ");
    QFinalState *sDone = new QFinalState(parent);
    global::TimedState *sWaitPolarityChanged = new global::TimedState(parent, 5000, "Attendre que la polarité change");

    // Print state changes
    PrintStateChanges(sReadCurrentPolarity, "ReadCurrentPolarity");
    PrintStateChanges(sWriteZeroSetPoint, "WriteZeroSetPoint");
    PrintStateChanges(sWaitPolarityChanged, "WaitPolarityChanged");
    PrintStateChanges(sDone, "Done");

    // parent
    QObject::connect(parent, &global::TimedState::SIGNAL_Timeout, this, &Controller::SIGNAL_Error);
    QObject::connect(parent, &QState::entered, this, [&]() {
        if (nsingle_->config().polarity_change_allowed()) {
            InfoMessage("Changement de polarité commencé");
        } else {
            emit SIGNAL_Error("Changement de polarité interdit pour cette alimentation");
        }} );
    parent->addTransition(this, SIGNAL(SIGNAL_Error(QString)), sDone);
    parent->setInitialState(sReadCurrentPolarity);

    // ReadCurrentPolarity
    QObject::connect(sReadCurrentPolarity, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sReadCurrentPolarity, &QState::entered, this, &Controller::ReadChannel1);
    sReadCurrentPolarity->addTransition(&regulated_, &RegulatedParameter::SIGNAL_ActUpdated, sWriteZeroSetPoint);

    // WriteZeroSetPoint
    QObject::connect(sWriteZeroSetPoint, &global::TimedState::SIGNAL_Timeout, this, &Controller::WarningMessage);
    QObject::connect(sWriteZeroSetPoint, &QState::entered, this, [&]() {
        regulated_.SetCurrentPolarityInvertedZeroDesValue();
        WriteAndVerifyChannel1SetPoint(regulated_.GetSetPoint());
    });
    if (nsingle_->config().verify_setpoints()) {
        sWriteZeroSetPoint->addTransition(&regulated_, &RegulatedParameter::SIGNAL_SetPointsMatch, sWaitPolarityChanged);
    } else {
        sWriteZeroSetPoint->addTransition(dynamic_cast<QObject*>(nsingle_), SIGNAL(SIGNAL_Channel1SetPoint(medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement)), sWaitPolarityChanged);
    }

    // WaitPolarityChanged
    QObject::connect(sWaitPolarityChanged, &QState::entered, this, &Controller::ReadChannel1);
    QObject::connect(sWaitPolarityChanged, &global::TimedState::SIGNAL_Timeout, this, [&]() { WarningMessage("Timeout attendant qu'alimentation change de polarité"); });
    auto tWaitPolarityChanged = sWaitPolarityChanged->addTransition(&regulated_, &RegulatedParameter::SIGNAL_PolarityCorrect, sDone);
    QObject::connect(tWaitPolarityChanged, &QSignalTransition::triggered, this, [&]() { InfoMessage("L'alimentation a changé de polarité"); });

    // Done
    // No actions
}


void Controller::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QAbstractState::entered, this, [&, name]() { qDebug() << "Controller:" << nsingle_->name() << " " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QAbstractState::exited, this, [&, name]() { qDebug() << "Controller:" <<  nsingle_->name() << " " << QDateTime::currentDateTime() << " <-" << name; });
}

} // ns
