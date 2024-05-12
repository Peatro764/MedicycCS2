#include "BobinePrincipaleCycler.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>
#include <QDBusConnection>
#include <QDebug>
#include <QSignalTransition>

#include "TimedState.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::bobine_principale_cycler {

BobinePrincipaleCycler::BobinePrincipaleCycler() :
    nsingle_repo_("_prod"),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    bp_levels_iter_ = bp_levels_.end();
    SetupEquipment();
    ConnectSignals();
    SetupStateStateMachine();
    SetupCycleStateMachine();
}

BobinePrincipaleCycler::~BobinePrincipaleCycler() {
    TearDownEquipment();
}

void BobinePrincipaleCycler::SetupEquipment() {
    qDebug() << "BobinePrincipaleCycler::SetupEquipment";
    try {
        hardware::nsingle::NSingleDbusAddress dbus = nsingle_repo_.GetNSingleDbusAddress("Bobine_Principale");
        controller_ = new medicyc::cyclotron::NSingleControllerInterface(dbus.interface_address(), dbus.controller_object_name(),
                                                                         medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);

        QObject::connect(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DemandForConfiguration, this, [&]() {
            DebugMessage("Le controlleur BobinePrincipale demand une configuration");
            if (!bp_levels_.empty()) {
                controller_->Configure(bp_levels_.begin()->setpoint, POLARITY);
            } else {
                ErrorMessage("Pas de niveau de cyclage disponible");
            }
        });
        if (!controller_->isValid()) {
            ErrorMessage("Impossible de se connecter à controlleur de la bobine principale");
        }
    } catch (std::exception& exc) {
        ErrorMessage(QString("Echec de l'obtention de l'adresse dbus de la bobine principale: ") + exc.what());
    }
}

void BobinePrincipaleCycler::TearDownEquipment() {
    qDebug() << "BobinePrincipaleCycler::TearDownEquipment";
    if (controller_) delete controller_;
}

void BobinePrincipaleCycler::ConnectSignals() {
    qDebug() << "BobinePrincipaleCycler::ConnectSignals";
    QObject::connect(this, &BobinePrincipaleCycler::SIGNAL_Error, this, &BobinePrincipaleCycler::ErrorMessage);
    // SIGNAL_Cyclage* used by external processes
    QObject::connect(this, &BobinePrincipaleCycler::SIGNAL_CommandStartup, this, &BobinePrincipaleCycler::SIGNAL_CyclageStarted);
    QObject::connect(this, &BobinePrincipaleCycler::SIGNAL_CommandCycle, this, &BobinePrincipaleCycler::SIGNAL_CyclageStarted);
    QObject::connect(this, &BobinePrincipaleCycler::SIGNAL_IterationFinished, this, &BobinePrincipaleCycler::SIGNAL_CyclageFinished);
}

void BobinePrincipaleCycler::SetupCycleStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        global::TimedState *sConfigure = new global::TimedState(sSuperState, 2000, "Echec d'obtention de la configuration", sIdle);
        QState *sStartup = new QState(sSuperState);
            global::TimedState *sSwitchOn = new global::TimedState(sStartup, 1400000, "Echec du passage au premier niveau de cyclage", sIdle);
            QState *sIterate = new QState(sStartup);
            global::TimedState *sMoveToLevel = new global::TimedState(sStartup, 280000, "Echoué à passer à un nouveau niveau de cyclage", sIdle);
            QState *sSleepOnTarget = new QState(sStartup);
        global::TimedState *sShutdown = new global::TimedState(sSuperState, 1400000, "Échec de l'arrêt", sIdle);

    // SuperState
    sSuperState->setInitialState(sConfigure);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandStartup, sStartup)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandCycle, sMoveToLevel)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandShutdown, sShutdown)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandInterrupt, sIdle)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandContinue, sSleepOnTarget)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Error, sIdle)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &BobinePrincipaleCycler::SIGNAL_Error, sIdle)->setTransitionType(QAbstractTransition::InternalTransition);

    // Configure
    QObject::connect(sConfigure, &QState::entered, this, [&]() { InfoMessage("Configuration en cours"); });
    QObject::connect(sConfigure, &global::TimedState::SIGNAL_Timeout, this, &BobinePrincipaleCycler::SIGNAL_Error);
    QObject::connect(sConfigure, &QState::entered, this, [&]() { SetCyclingLevels(nsingle_repo_.GetBPLevels()); });
    sConfigure->addTransition(sConfigure, &QState::entered, sIdle);

    // Idle
    // no further actions

    // Startup
    sStartup->setInitialState(sSwitchOn);
    // transitions that trigger a SIGNAL_CyclageInterrupted when any transition from startup state either than
    // SIGNAL_IterationFinished is issued
    auto tShutdownInStartup = sStartup->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandShutdown, sShutdown);
    QObject::connect(tShutdownInStartup, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_CyclageInterrupted(); });

    auto tInterruptInStartup = sStartup->addTransition(this, &BobinePrincipaleCycler::SIGNAL_CommandInterrupt, sIdle);
    QObject::connect(tInterruptInStartup, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_CyclageInterrupted(); });

    auto tErrorInStartup = sStartup->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Error, sIdle);
    QObject::connect(tErrorInStartup, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_CyclageInterrupted(); });

        // SwitchOn
        QObject::connect(sSwitchOn, &global::TimedState::SIGNAL_Timeout, this, &BobinePrincipaleCycler::SIGNAL_Error);
        QObject::connect(sSwitchOn, &QState::entered, this, [&]() {
            controller_->Startup(); });
        sSwitchOn->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, sSleepOnTarget);//sMoveToLevel); TODO CHECK IF OK

        // Iterate
        QObject::connect(sIterate, &QState::entered, this, &BobinePrincipaleCycler::Iterate);
        auto tFinished = sIterate->addTransition(this, &BobinePrincipaleCycler::SIGNAL_IterationFinished, sIdle);
        QObject::connect(tFinished, &QSignalTransition::triggered, this, [&]() {
            emit SIGNAL_StartupFinished();
            InfoMessage("Démarrage terminé");
        });
        sIterate->addTransition(this, &BobinePrincipaleCycler::SIGNAL_Iterate, sMoveToLevel);

        // MoveToLevel
        QObject::connect(sMoveToLevel, &global::TimedState::SIGNAL_Timeout, this, &BobinePrincipaleCycler::SIGNAL_Error);
        QObject::connect(sMoveToLevel, &QState::entered, this, [&]() {
            controller_->UpdateDesiredValue(bp_levels_iter_->setpoint); });
        sMoveToLevel->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, sSleepOnTarget);

        // SleepOnTarget
        QObject::connect(sSleepOnTarget, &QState::entered, [&]() { timer_sleep_.start(bp_levels_iter_->duration  * 1000); });
        QObject::connect(sSleepOnTarget, &QState::exited, &timer_sleep_, &QTimer::stop);
        sSleepOnTarget->addTransition(&timer_sleep_, &QTimer::timeout, sIterate);

    // Shutdown
    QObject::connect(sShutdown, &global::TimedState::SIGNAL_Timeout, this, &BobinePrincipaleCycler::SIGNAL_Error);
    QObject::connect(sShutdown, &QState::entered, this, [&]() { controller_->Shutdown(); });
    auto tShutdownFinished = sShutdown->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, sIdle);
    QObject::connect(tShutdownFinished, &QSignalTransition::triggered, this, [&]() {
        emit SIGNAL_ShutdownFinished();
        InfoMessage("Arrêt terminé");
    });

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sConfigure, "Configure");
    PrintStateChanges(sStartup, "Startup");
    PrintStateChanges(sSwitchOn, "SwitchOn");
    PrintStateChanges(sIterate, "Iterate");
    PrintStateChanges(sMoveToLevel, "MoveToLevel");
    PrintStateChanges(sSleepOnTarget, "SleepOnTarget");
    PrintStateChanges(sShutdown, "Shutdown");

    sm_cycle_.addState(sSuperState);
    sm_cycle_.setInitialState(sSuperState);
    sm_cycle_.start();
}

void BobinePrincipaleCycler::SetupStateStateMachine() {
    QState *sSuperState = new QState();
        global::TimedState *sOff = new global::TimedState(sSuperState, 3000, "");
        global::TimedState *sIntermediate = new global::TimedState(sSuperState, 3000, "");
        global::TimedState *sReady = new global::TimedState(sSuperState, 3000, "");
        global::TimedState *sUnknown = new global::TimedState(sSuperState, 3000, "");

    // SuperState
    sSuperState->setInitialState(sUnknown);
    sSuperState->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, sOff)->setTransitionType(QAbstractTransition::InternalTransition);

    // Off
    QObject::connect(sOff, &QState::entered, this, &BobinePrincipaleCycler::SIGNAL_State_Off);
    sOff->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, sIntermediate);

    // Intermediate
    QObject::connect(sIntermediate, &QState::entered, this, &BobinePrincipaleCycler::SIGNAL_State_Intermediate);
    sIntermediate->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, sReady);

    // sReady
    QObject::connect(sReady, &QState::entered, this, &BobinePrincipaleCycler::SIGNAL_State_Ready);
    sReady->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, sIntermediate);

    // Unknown
    QObject::connect(sUnknown, &QState::entered, this, &BobinePrincipaleCycler::SIGNAL_State_Unknown);
    sUnknown->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, sIntermediate);
    sUnknown->addTransition(controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, sReady);

    sm_state_.addState(sSuperState);
    sm_state_.setInitialState(sSuperState);
    sm_state_.start();
}

void BobinePrincipaleCycler::Iterate() {
    if (!bp_levels_.empty() && bp_levels_iter_ != bp_levels_.end()) {
        ++bp_levels_iter_;
        if (bp_levels_iter_ != bp_levels_.end()) {
            emit SIGNAL_Iterate();
        } else {
            emit SIGNAL_IterationFinished();
        }
    } else {
        emit SIGNAL_Error("Il n'y a pas d'étape configurée");
    }
}

void BobinePrincipaleCycler::SetCyclingLevels(std::vector<nsingle::BPLevel> levels) {
    qDebug() << "BobinePrincipaleCycler::SetCyclingLevels " << levels.size();
    bp_levels_.clear();
    if (!levels.empty()) {
        bp_levels_ = levels;
        bp_levels_iter_ = bp_levels_.begin();
        controller_->Configure(bp_levels_.begin()->setpoint, POLARITY);
    } else {
        emit SIGNAL_Error("Aucun niveau de cyclage trouvé dans la configuration.");
    }
}

void BobinePrincipaleCycler::Startup() {
    InfoMessage("Demarrage commandé");
    if (!controller_->isValid()) {
        emit SIGNAL_Error("Pas de connexion dbus à contrôleur nsingle");
        return;
    }
    if (!bp_levels_.empty()) {
        bp_levels_iter_ = bp_levels_.begin();
        emit SIGNAL_CommandStartup();
    } else {
        emit SIGNAL_Error("Il n'y a pas de niveaux de cyclages configurées");
    }
}

void BobinePrincipaleCycler::Cycle() {
    InfoMessage("Cyclage commandé");
    if (!controller_->isValid()) {
        emit SIGNAL_Error("Pas de connexion dbus à contrôleur nsingle");
        return;
    }
    if (!bp_levels_.empty()) {
        bp_levels_iter_ = bp_levels_.begin();
        emit SIGNAL_CommandCycle();
    } else {
        emit SIGNAL_Error("Il n'y a pas d'étapes configurées");
    }
}

void BobinePrincipaleCycler::Shutdown() {
    InfoMessage("Arrêt commandé");
    if (!controller_->isValid()) {
        emit SIGNAL_Error("Pas de connexion dbus à contrôleur nsingle");
        return;
    }
    emit SIGNAL_CommandShutdown();
}

void BobinePrincipaleCycler::Interrupt() {
    InfoMessage("Interruption commandé");
    if (!controller_->isValid()) {
        emit SIGNAL_Error("Pas de connexion dbus à contrôleur nsingle");
        return;
    }
    controller_->Interrupt();
    emit SIGNAL_CommandInterrupt();
}

void BobinePrincipaleCycler::Continue() {
    InfoMessage("Continuation commandé");
    if (!controller_->isValid()) {
        emit SIGNAL_Error("Pas de connexion dbus à contrôleur nsingle");
        return;
    }
    emit SIGNAL_CommandContinue();
}

void BobinePrincipaleCycler::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Bobine Principale"), message);
}

void BobinePrincipaleCycler::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Bobine Principale"), message);
}

void BobinePrincipaleCycler::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Bobine Principale"), message);
}

void BobinePrincipaleCycler::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Bobine Principale"), message);
}

void BobinePrincipaleCycler::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "BP " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "BP " << QDateTime::currentDateTime() << " <-" << name; });
}


}
