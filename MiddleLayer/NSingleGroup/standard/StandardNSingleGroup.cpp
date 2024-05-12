#include "StandardNSingleGroup.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>

#include "TimedState.h"
#include "Enumerations.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard {

StandardNSingleGroup::StandardNSingleGroup(QString sub_system, int iteration_delay) :
    global_repo_("_prod"),
    nsingle_repo_("_prod"),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    sub_system_(sub_system),
    iteration_delay_(iteration_delay)
{
    qDebug() << sub_system_ << "::" << sub_system_;
    SetupStateMachine();
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Ready,this, &StandardNSingleGroup::SIGNAL_State_Ready);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Off,this, &StandardNSingleGroup::SIGNAL_State_Off);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Unknown,this, &StandardNSingleGroup::SIGNAL_State_Unknown);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Intermediate,this, &StandardNSingleGroup::SIGNAL_State_Intermediate);
    GetLastActiveConfig();
}

StandardNSingleGroup::~StandardNSingleGroup() {
    qDebug() << sub_system_ << "::~" << sub_system_;
    UnConfigure();
}

void StandardNSingleGroup::GetLastActiveConfig() {
    try {
        demanded_cyclo_config_ = global_repo_.GetActiveConfig(sub_system_);
        SetupControllers();
        emit SIGNAL_Configuration(demanded_cyclo_config_);
    } catch (std::exception& exc) {
        qWarning() << "StandardNSingleGroup::GetLastActiveConfig Exception thrown: " << exc.what();
        ErrorMessage("La dernière configuration active est introuvable. Veuillez charger une nouvelle configuration");
    }
}

void StandardNSingleGroup::Configure(QString cyclo_config) {
    demanded_cyclo_config_ = cyclo_config;
    emit SIGNAL_CommandConfigure();
}

void StandardNSingleGroup::QueryConfiguration() {
    qDebug() << "StandardNSingleGroup::QueryConfiguration";
    emit SIGNAL_Configuration(active_cyclo_config_);
}

void StandardNSingleGroup::SetupControllers() {
    try {
        UnConfigure();
        qDebug() << "StandardNSingleGroup::SetupControllers";
        InfoMessage("Nouvelle configuration : " + demanded_cyclo_config_);
        QMap<QString, bool> nsingles = nsingle_repo_.GetNSingles(sub_system_, demanded_cyclo_config_);
        if (nsingles.empty()) throw std::runtime_error("Aucun nsingle trouvé");

        for (QString s : nsingles.keys()) {
            qDebug() << sub_system_ << "::Configure setting up subsystem " << s << " with configuration " << demanded_cyclo_config_;
            if (!nsingles.value(s)) {
                ErrorMessage("La configuration choisie nécessite un nsingle qui n'est pas disponible: " + s);
            }
            nsingle_setpoints_[s] = nsingle_repo_.GetNSingleSetPoint(demanded_cyclo_config_, s);
            qDebug() << "StandardNSingleGroup::SetupControllers NSingleSetPoint: "
                     << s
                     << " enabled " << nsingle_setpoints_[s].powered_up()
                     << " sign " << nsingle_setpoints_[s].sign()
                     << " value " << nsingle_setpoints_[s].value();

            nsingle::NSingleDbusAddress dbus = nsingle_repo_.GetNSingleDbusAddress(s);
            nsingle_controllers_[s] = new medicyc::cyclotron::NSingleControllerInterface(dbus.interface_address(), dbus.controller_object_name(),
                                                                                         medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            group_state_.AddEquipment(s, nsingle_setpoints_.value(s).powered_up());
            QObject::connect(nsingle_controllers_.value(s), &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [&, s]() {
                group_state_.ChangeState(s, utils::EquipmentState::OFF);
            });
            QObject::connect(nsingle_controllers_.value(s), &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [&, s]() {
                group_state_.ChangeState(s, utils::EquipmentState::ON);
            });
            QObject::connect(nsingle_controllers_.value(s), &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, this, [&, s]() {
                group_state_.ChangeStatus(s, utils::EquipmentStatus::ONTARGET);
            });
            QObject::connect(nsingle_controllers_.value(s), &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, this, [&, s]() {
                group_state_.ChangeStatus(s, utils::EquipmentStatus::OFFTARGET);
            });

            QObject::connect(nsingle_controllers_.value(s), &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DemandForConfiguration, this, [&, s]() {
                qDebug() << QString("Le controlleur ") + s + " demand une configuration";
                InfoMessage("Le controlleur " + s + " demand une configuration");
                nsingle_controllers_.value(s)->Configure(nsingle_setpoints_.value(s).value(), nsingle_setpoints_.value(s).sign());
            });
        }
        active_cyclo_config_ = demanded_cyclo_config_;
        demanded_cyclo_config_.clear();
    } catch(std::exception& exc) {
        demanded_cyclo_config_.clear();
        UnConfigure();
        qWarning() << sub_system_ << "::Configure Exception caught: " << exc.what();
        ErrorMessage(QString("La configuration a échoué: ") + exc.what());
    }
}

void StandardNSingleGroup::ApplyConfiguration() {
    qDebug() << "StandardNSingleGroup::ApplyConfiguration";
    for (auto& s : nsingle_controllers_.keys()) {
        qDebug() << "Controller " << s;
        if (nsingle_setpoints_.contains(s)) {
            nsingle_controllers_.value(s)->Configure(nsingle_setpoints_.value(s).value(), nsingle_setpoints_.value(s).sign());
        } else {
            WarningMessage(QString("Pas de consigne trouvé pour: ") + s);
        }
    }
}

void StandardNSingleGroup::UnConfigure() {
    qDebug() << "StandardNSingleGroup::UnConfigure";
    for (auto& nsingle : nsingle_controllers_.keys()) {
        QObject::disconnect(nsingle_controllers_.value(nsingle));
        delete nsingle_controllers_.value(nsingle);
    }
    nsingle_controllers_.clear();
    nsingle_setpoints_.clear();
    active_cyclo_config_.clear();
    group_state_.ClearEquipments();
}

void StandardNSingleGroup::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        QState *sConfigure = new QState(sSuperState);
        global::TimedState *sStartup = new global::TimedState(sSuperState, 120000, "Échec de démarrage", sIdle);
        global::TimedState *sWaitStartUpFinished = new global::TimedState(sSuperState, 240000, "Timeout en attente de démarrage des alimentations", sIdle);
        global::TimedState *sShutdown = new global::TimedState(sSuperState, 120000, "Échec de l'arrêt", sIdle);
        global::TimedState *sWaitShutDownFinished = new global::TimedState(sSuperState, 120000, "Échec de l'arrêt", sIdle);

    // SuperState
    sSuperState->setInitialState(sIdle);
    sSuperState->addTransition(this, &StandardNSingleGroup::SIGNAL_CommandStartup, sStartup)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &StandardNSingleGroup::SIGNAL_CommandShutdown, sShutdown)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &StandardNSingleGroup::SIGNAL_CommandInterrupt, sIdle)->setTransitionType(QAbstractTransition::InternalTransition);
    sSuperState->addTransition(this, &StandardNSingleGroup::SIGNAL_CommandConfigure, sConfigure)->setTransitionType(QAbstractTransition::InternalTransition);

    // Idle
    // no further transitions    

    // Configure
    QObject::connect(sConfigure, &QState::entered, this, &StandardNSingleGroup::SetupControllers);
    QObject::connect(sConfigure, &QState::entered, this, &StandardNSingleGroup::ApplyConfiguration);
    sConfigure->addTransition(sConfigure, &QState::entered, sIdle);

    // Startup
    QObject::connect(sStartup, &global::TimedState::SIGNAL_Timeout, this, &StandardNSingleGroup::ErrorMessage);
    QObject::connect(sStartup, &QState::entered, this, &StandardNSingleGroup::StartupIteratively);
    sStartup->addTransition(this, &StandardNSingleGroup::SIGNAL_Iterate, sStartup);
    sStartup->addTransition(this, &StandardNSingleGroup::SIGNAL_IterationFinished, sWaitStartUpFinished);

    // WaitStartupFinished
    QObject::connect(sWaitStartUpFinished, &global::TimedState::SIGNAL_Timeout, this, &StandardNSingleGroup::ErrorMessage);
    auto tStartupFinished = sWaitStartUpFinished->addTransition(&group_state_, &utils::EquipmentGroupState::SIGNAL_Ready, sIdle);
    QObject::connect(tStartupFinished, &QSignalTransition::triggered, this, [&]() {
        emit SIGNAL_StartupFinished();
        InfoMessage("Démarrage términé");
    });

    // Shutdown
    QObject::connect(sShutdown, &global::TimedState::SIGNAL_Timeout, this, &StandardNSingleGroup::ErrorMessage);
    QObject::connect(sShutdown, &QState::entered, this, &StandardNSingleGroup::ShutdownIteratively);
    sShutdown->addTransition(this, &StandardNSingleGroup::SIGNAL_Iterate, sShutdown);
    sShutdown->addTransition(this, &StandardNSingleGroup::SIGNAL_IterationFinished, sWaitShutDownFinished);

    // WaitShutdownFinished
    QObject::connect(sWaitShutDownFinished, &global::TimedState::SIGNAL_Timeout, this, &StandardNSingleGroup::ErrorMessage);
    auto tShutdownFinished = sWaitShutDownFinished->addTransition(&group_state_, &utils::EquipmentGroupState::SIGNAL_Off, sIdle);
    QObject::connect(tShutdownFinished, &QSignalTransition::triggered, this, [&]() {
        InfoMessage("Arrêt términé");
        emit SIGNAL_ShutdownFinished();
    });

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sStartup, "Startup");
    PrintStateChanges(sWaitStartUpFinished, "WaitStartupFinished");
    PrintStateChanges(sShutdown, "Shutdown");
    PrintStateChanges(sWaitShutDownFinished, "WaitShutdownFinished");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void StandardNSingleGroup::StartupIteratively() {
    qDebug() << sub_system_ << "::StartupIteratively ";
    if (iter_ != nsingle_setpoints_.end()) {
        auto controller = nsingle_controllers_.value(iter_.key());
        if (controller->isValid()) {
            if (iter_->powered_up()) {
                controller->Startup();
            } else {
                controller->Shutdown();
            }
        } else {
            ErrorMessage("Pas de connexion dbus au controlleur de " + iter_.key() + ", démarrage interrompu");
        }
        ++iter_;
        QTimer::singleShot(iteration_delay_, this, &StandardNSingleGroup::SIGNAL_Iterate);
    } else {
        emit SIGNAL_IterationFinished();
    }
}

void StandardNSingleGroup::ShutdownIteratively() {
    qDebug() << sub_system_ << "::ShutdownIteratively";
    if (iter_ != nsingle_setpoints_.end()) {
        auto controller = nsingle_controllers_.value(iter_.key());
        if (controller->isValid()) {
            controller->Shutdown();
            QTimer::singleShot(iteration_delay_, this, &StandardNSingleGroup::SIGNAL_Iterate);
        } else {
            ErrorMessage("Pas de connexion dbus au controlleur de " + iter_.key() + ", arrêt interrompu");
            QTimer::singleShot(100, this, &StandardNSingleGroup::SIGNAL_Iterate);
        }
        ++iter_;
    } else {
        emit SIGNAL_IterationFinished();
    }
}

void StandardNSingleGroup::Startup() {
    try {
        qDebug() << sub_system_ << "::Startup";
        global_repo_.SetActiveConfig(sub_system_, active_cyclo_config_);
        emit SIGNAL_Configuration(active_cyclo_config_);
        emit SIGNAL_CommandInterrupt();
        if (IsConfigured()) {
            InfoMessage("Demarrage commandé");
            iter_ = nsingle_setpoints_.begin();
            emit SIGNAL_CommandStartup();
        } else {
            ErrorMessage("Sélectionner une configuration avant de démarrer");
        }
    } catch (std::exception& exc) {
        qDebug() << "StandardNSingleGroup::Startup exception thrown: " << exc.what();
        ErrorMessage(QString("Erreur ") + exc.what());
    }
}

void StandardNSingleGroup::Shutdown() {
    qDebug() << sub_system_ << "::Shutdown";
    emit SIGNAL_CommandInterrupt();
    if (IsConfigured()) {
        InfoMessage("Arrêt commandé");
        iter_ = nsingle_setpoints_.begin();
        emit SIGNAL_CommandShutdown();
    } else {
        ErrorMessage("Sélectionner une configuration avant d'arrêter");
    }
}

void StandardNSingleGroup::Interrupt() {
    qDebug() << sub_system_ << "::Interrupt";
    emit SIGNAL_CommandInterrupt();
    InfoMessage("Interruption commandé");
    for (auto name : nsingle_setpoints_.keys()) {
        auto controller = nsingle_controllers_[name];
        if (controller->isValid()) {
            controller->Interrupt();
        } else {
            ErrorMessage("Pas de connexion dbus au controlleur de " + name + ", command interrompu");
        }
    }
}

void StandardNSingleGroup::ErrorMessage(QString message) {
    qDebug() << message;
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), sub_system_, message);
}

void StandardNSingleGroup::WarningMessage(QString message) {
    qDebug() << message;
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), sub_system_, message);
}

void StandardNSingleGroup::InfoMessage(QString message) {
    qDebug() << message;
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), sub_system_, message);
}

void StandardNSingleGroup::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << sub_system_ << " " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << sub_system_ << " " << QDateTime::currentDateTime() << " <-" << name; });
}

bool StandardNSingleGroup::IsConfigured() const {
    return !active_cyclo_config_.isEmpty();
}

}
