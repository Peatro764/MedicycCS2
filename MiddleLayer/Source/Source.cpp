#include "Source.h"

#include <QStandardPaths>
#include <QSignalTransition>
#include <QDateTime>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::source {

namespace global = medicyc::cyclotroncontrolsystem::global;

Source::Source() :
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    global_repo_("_prod"),
    nsingle_repo_("_prod"),
    source_repo_("_prod")
{
    qDebug() << "Source::Source";
    SetupDbusConnections();
    SetupGroupState();
    SetupStateMachine();
    GetLastActiveConfig();
    ExecuteConfiguration();
}

Source::~Source()
{
    delete injection_;
    delete extraction_;
    delete arc_;
    delete filament_;
}

// Public interface

void Source::Configure(QString name) {
    qDebug() << "Source::Configure " << name;
    DebugMessage("Nouvelle configuration demandée: " + name);
    demanded_cyclo_config_ = name;
    emit SIGNAL_CommandConfigure();
}

void Source::QueryConfiguration() {
    qDebug() << "Source::QueryConfiguration";
    emit SIGNAL_Configuration(active_cyclo_config_);
}

void Source::Startup() {
    InfoMessage("Demarrage commandé");
    emit SIGNAL_Configuration(active_cyclo_config_);
    emit SIGNAL_CommandStartup();
}

void Source::Shutdown() {
    InfoMessage("Arrêt commandé");
    emit SIGNAL_CommandShutdown();
}

void Source::Interrupt() {
    InfoMessage("Interruption commandé");
    extraction_->Interrupt();
    filament_->Interrupt();
    arc_->Interrupt();
    injection_->Interrupt();
    emit SIGNAL_CommandInterrupt();
}

// Private

void Source::GetLastActiveConfig() {
    try {
        qDebug() << "Source::GetLastActiveConfig";
        demanded_cyclo_config_ = global_repo_.GetActiveConfig(global::Enumerations::SubSystemToString(sub_system_));
    } catch (std::exception& exc) {
        qWarning() << "Source::GetLastActiveConfig Exception thrown: " << exc.what();
        ErrorMessage("La dernière configuration active est introuvable. Veuillez charger une nouvelle configuration");
    }
}

void Source::ExecuteConfiguration() {
    try {
        qDebug() << "Source::ExecuteConfiguration";
        if (!demanded_cyclo_config_.isEmpty()) {
            active_cyclo_config_ = demanded_cyclo_config_;
            ConfigureExtraction();
            ConfigureArc();
            ConfigureFilament();
            ConfigureInjection33kev();
        } else {
            ErrorMessage(QString("Pas de configuration"));
        }
    } catch (std::exception& exc) {
        qWarning() << "Source::ExecuteConfiguration Exception thrown " << exc.what();
        ErrorMessage(QString("La configuration a échoué: " ) + exc.what());
    }
}

void Source::ConfigureInjection33kev() {
    try {
        qDebug() << "Source::ConfigureInjection33kev";
        if (!active_cyclo_config_.isEmpty()) {
            DebugMessage("Configurer l'injection33kev");
            const double injection_setpoint = source_repo_.GetSetPoint("Injection", "V", active_cyclo_config_);
            injection_->Configure(injection_setpoint, true); // TODO: Check polarity
        } else {
            ErrorMessage("Pas de configuration, abandonner");
        }
    } catch (std::exception& exc) {
        qWarning() << "Source::ConfigureInjection33kev Exception thrown " << exc.what();
        ErrorMessage(QString("La configuration de l'injection33kev a échoué: " ) + exc.what());
    }
}

void Source::ConfigureExtraction() {
    try {
        qDebug() << "Source::ConfigureExtraction";
        if (!active_cyclo_config_.isEmpty()) {
            DebugMessage("Configurer l'extraction");
            const double extraction_setpoint = source_repo_.GetSetPoint("Extraction", "V", demanded_cyclo_config_);
            extraction_->Configure(extraction_setpoint);
        } else {
            ErrorMessage("Pas de configuration, abandonner");
        }
    } catch (std::exception& exc) {
        qWarning() << "Source::ConfigureExtraction Exception thrown " << exc.what();
        ErrorMessage(QString("La configuration de l'extraction a échoué: " ) + exc.what());
    }
}

void Source::ConfigureArc() {
    try {
        qDebug() << "Source::ConfigureArc";
        if (!active_cyclo_config_.isEmpty()) {
            DebugMessage("Configurer l'arc");
            const double arc_v_setpoint = source_repo_.GetSetPoint("Arc", "V", demanded_cyclo_config_);
            arc_->Configure(arc_v_setpoint);
        } else {
            ErrorMessage("Pas de configuration, abandonner");
        }
    } catch (std::exception& exc) {
        qWarning() << "Source::ConfigureArc Exception thrown " << exc.what();
        ErrorMessage(QString("La configuration de l'arc a échoué: " ) + exc.what());
    }
}

void Source::ConfigureFilament() {
    try {
        qDebug() << "Source::ConfigureFilament";
        if (!active_cyclo_config_.isEmpty()) {
            DebugMessage("Configurer le filament");
            const double arc_i_setpoint = source_repo_.GetSetPoint("Arc", "I", demanded_cyclo_config_);
            filament_->Configure(arc_i_setpoint);
        } else {
            ErrorMessage("Pas de configuration, abandonner");
        }
    } catch (std::exception& exc) {
        qWarning() << "Source::ConfigureFilament Exception thrown " << exc.what();
        ErrorMessage(QString("La configuration du filament a échoué: " ) + exc.what());
    }
}

void Source::SetupDbusConnections() {
    try {
        if (injection_) delete injection_;
        if (extraction_) delete extraction_;
        if (arc_) delete arc_;
        if (filament_) delete filament_;
        qDebug() << "Source::SetupDbusConnections";
        hw_nsingle::NSingleDbusAddress inj_dbus = nsingle_repo_.GetNSingleDbusAddress("Injection_33kV");
        hw_sourcepowersupply::PowerSupplyDbusAddress ext_dbus = source_repo_.GetDbusAddress("Extraction");
        hw_sourcepowersupply::PowerSupplyDbusAddress fil_dbus = source_repo_.GetDbusAddress("Filament");
        hw_sourcepowersupply::PowerSupplyDbusAddress arc_dbus = source_repo_.GetDbusAddress("Arc");
        injection_ = new medicyc::cyclotron::NSingleControllerInterface(inj_dbus.interface_address(), inj_dbus.controller_object_name(), medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
        extraction_ = new medicyc::cyclotron::SourcePowerSupplyInterface(ext_dbus.interface_address(), ext_dbus.object_name(), medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
        arc_ = new medicyc::cyclotron::SourcePowerSupplyInterface(arc_dbus.interface_address(), arc_dbus.object_name(), medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
        filament_ = new medicyc::cyclotron::SourcePowerSupplyInterface(fil_dbus.interface_address(), fil_dbus.object_name(), medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
        qDebug() << QString("Source::SetupDbusConnections Injection33kV dbus ") + (injection_->isValid() ? "valid" : "not valid");
        qDebug() << QString("Source::SetupDbusConnections Extraction dbus ") + (extraction_->isValid() ? "valid" : "not valid");
        qDebug() << QString("Source::SetupDbusConnections Filament dbus ") + (filament_->isValid() ? "valid" : "not valid");
        qDebug() << QString("Source::SetupDbusConnections Arc dbus ") + (arc_->isValid() ? "valid" : "not valid");

        QObject::connect(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DemandForConfiguration, this, &Source::ConfigureInjection33kev);
        QObject::connect(extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_DemandForConfiguration, this, &Source::ConfigureExtraction);
        QObject::connect(arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_DemandForConfiguration, this, &Source::ConfigureArc);
        QObject::connect(filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_DemandForConfiguration, this, &Source::ConfigureFilament);

        QTimer *timer = new QTimer(this);
        timer->setInterval(3000);
        timer->setSingleShot(false);
        timer->start();
        QObject::connect(timer, &QTimer::timeout, this, &Source::CheckDbusConnections);
    } catch(std::exception& exc) {
        qWarning() << "Source::SetupDbusConnections Exception caught, exiting:  " << exc.what();
        exit(-1);
    }
}

void Source::CheckDbusConnections() {
    qDebug() << "Source::CheckDbusConnections";
    bool dbus_ok = (arc_->isValid() &&
                    extraction_->isValid() &&
                    filament_->isValid() &&
                    injection_->isValid());
    if (dbus_ok) {
        emit SIGNAL_DbusOk();
    } else {
        emit SIGNAL_DbusError();
    }
}

void Source::SetupGroupState() {
    qDebug() << "Source::SetupGroupState";

    auto AddSourcePowerSupplyState = [&](QString name, medicyc::cyclotron::SourcePowerSupplyInterface* iface) {
        iface->setObjectName(name);
        group_state_.AddEquipment(name, true);
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, this, [iface, this]() {
            group_state_.ChangeState(iface->objectName(), utils::EquipmentState::OFF);
        });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_On, this, [iface, this]() {
            group_state_.ChangeState(iface->objectName(), utils::EquipmentState::ON);
        });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_OffTarget, this, [iface, this]() {
            group_state_.ChangeStatus(iface->objectName(), utils::EquipmentStatus::OFFTARGET);
        });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_OnTarget, this, [iface, this]() {
            group_state_.ChangeStatus(iface->objectName(), utils::EquipmentStatus::ONTARGET);
        });
    };

    AddSourcePowerSupplyState("Arc",arc_);
    AddSourcePowerSupplyState("Extraction",extraction_);
    AddSourcePowerSupplyState("Filament",filament_);

    // injection
    injection_->setObjectName("Injection");
    group_state_.AddEquipment(injection_->objectName(), true);
    QObject::connect(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [&]() {
        group_state_.ChangeState(injection_->objectName(), utils::EquipmentState::OFF);
    });
    QObject::connect(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [&]() {
        group_state_.ChangeState(injection_->objectName(), utils::EquipmentState::ON);
    });
    QObject::connect(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, this, [&]() {
        group_state_.ChangeStatus(injection_->objectName(), utils::EquipmentStatus::OFFTARGET);
    });
    QObject::connect(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, this, [&]() {
        group_state_.ChangeStatus(injection_->objectName(), utils::EquipmentStatus::ONTARGET);
    });

    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Ready,this, &Source::SIGNAL_State_Ready);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Off,this, &Source::SIGNAL_State_Off);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Unknown,this, &Source::SIGNAL_State_Unknown);
    QObject::connect(&group_state_, &utils::EquipmentGroupState::SIGNAL_Intermediate,this, &Source::SIGNAL_State_Intermediate);
}

void Source::SetupStateMachine() {
    qDebug() << "Source::SetupStateMachine";
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        global::TimedState *sConfigure = new global::TimedState(sSuperState, 3000, "La configuration a échoué", sIdle);
        global::TimedState *sStartup = new global::TimedState(sSuperState, 240000, "Échec de démarrage Source", sIdle);
            global::TimedState *sStartupExtraction = new global::TimedState(sStartup, 120000, "Échec de démarrage Extraction", sIdle);
            global::TimedState *sStartupArc = new global::TimedState(sStartup, 120000, "Échec de démarrage Arc", sIdle);
            global::TimedState *sStartupFilament = new global::TimedState(sStartup, 120000, "Échec de démarrage Filament", sIdle);
            // Let operators startup injection as usual
//            global::TimedState *sStartupInjection = new global::TimedState(sStartup, 60000, "Échec de démarrage Injection", sIdle);
        global::TimedState *sShutdown = new global::TimedState(sSuperState, 120000, "Échec de l'arrêt Source", sIdle);
            global::TimedState *sShutdownInjection = new global::TimedState(sShutdown, 60000, "Échec de l'arrêt Injection", sIdle);
            global::TimedState *sShutdownFilament = new global::TimedState(sShutdown, 60000, "Échec de l'arrêt Filament", sIdle);
            global::TimedState *sShutdownArc = new global::TimedState(sShutdown, 60000, "Échec de l'arrêt Arc", sIdle);
            global::TimedState *sShutdownExtraction = new global::TimedState(sShutdown, 60000,"Échec de l'arrêt Extraction", sIdle);

        // SuperState
        sSuperState->setInitialState(sIdle);
        auto tArcError = sSuperState->addTransition(arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error, sIdle);
        auto tExtractionError = sSuperState->addTransition(extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error, sIdle);
        auto tFilamentError = sSuperState->addTransition(filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error, sIdle);
        auto tInjectionError = sSuperState->addTransition(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Error, sIdle);
        QObject::connect(tArcError, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Erreur de sous-système Arc, abandon des processus en cours"); } );
        QObject::connect(tExtractionError, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Erreur de sous-système Extraction, abandon des processus en cours"); } );
        QObject::connect(tFilamentError, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Erreur de sous-système Filament, abandon des processus en cours"); } );
        QObject::connect(tInjectionError, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Erreur de sous-système Injection, abandon des processus en cours"); } );
        auto tDbusError = sSuperState->addTransition(this, &Source::SIGNAL_DbusError, sIdle);
        QObject::connect(tDbusError, &QSignalTransition::triggered, this, [&]() {
            Interrupt();
            ErrorMessage("Dbus déconnecté, tous les processus interrompus");
        });
        sSuperState->addTransition(this, &Source::SIGNAL_CommandConfigure, sConfigure)->setTransitionType(QAbstractTransition::InternalTransition);
        sSuperState->addTransition(this, &Source::SIGNAL_CommandStartup, sStartup)->setTransitionType(QAbstractTransition::InternalTransition);
        sSuperState->addTransition(this, &Source::SIGNAL_CommandShutdown, sShutdown)->setTransitionType(QAbstractTransition::InternalTransition);
        sSuperState->addTransition(this, &Source::SIGNAL_CommandInterrupt, sIdle)->setTransitionType(QAbstractTransition::InternalTransition);

        // Idle
        // Prevent these signals to trigger a reenter into idle. Eat them silently
        sIdle->addTransition(new QSignalTransition(this, &Source::SIGNAL_DbusError));
        sIdle->addTransition(new QSignalTransition(arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error));
        sIdle->addTransition(new QSignalTransition(filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error));
        sIdle->addTransition(new QSignalTransition(extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Error));
        sIdle->addTransition(new QSignalTransition(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Error));

        // Configure
        QObject::connect(sConfigure, &QState::entered, this, &Source::ExecuteConfiguration);
        sConfigure->addTransition(sConfigure, &QState::entered, sIdle);

        // Startup Source
        QObject::connect(sStartup, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
        sStartup->setInitialState(sStartupArc);

            // Startup Arc
            QObject::connect(sStartupArc, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
            QObject::connect(sStartupArc, &global::TimedState::entered, arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::Startup);
            sStartupArc->addTransition(arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_OnTarget, sStartupExtraction);

            // Startup Extraction
            QObject::connect(sStartupExtraction, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
            QObject::connect(sStartupExtraction, &global::TimedState::entered, extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::Startup);
            sStartupExtraction->addTransition(extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_OnTarget, sStartupFilament);

            // Startup Filament
            QObject::connect(sStartupFilament, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
            QObject::connect(sStartupFilament, &global::TimedState::entered, filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::Startup);
            auto tStartupFinished = sStartupFilament->addTransition(filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_OnTarget, sIdle);
            QObject::connect(tStartupFinished, &QSignalTransition::triggered, this, [&]() {
                InfoMessage("Démarrage terminé, le 33 kV peut être réglé");
                emit SIGNAL_StartupFinished();
            });

            // Startup Injection
//            QObject::connect(sStartupInjection, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
//            QObject::connect(sStartupInjection, &global::TimedState::entered, injection_, &medicyc::cyclotron::NSingleControllerInterface::Startup);
//            auto sStartupFinished = sStartupInjection->addTransition(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, sIdle);
//            QObject::connect(sStartupFinished, &QSignalTransition::triggered, this, [&]() {
//                InfoMessage("Démarrage terminé");
//                emit SIGNAL_StartupFinished();
//            });

        // Shutdown
        QObject::connect(sShutdown, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
        sShutdown->setInitialState(sShutdownInjection);

                QObject::connect(sShutdownInjection, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
                QObject::connect(sShutdownInjection, &QState::entered, injection_, &medicyc::cyclotron::NSingleControllerInterface::Shutdown);
                sShutdownInjection->addTransition(injection_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, sShutdownExtraction);

                QObject::connect(sShutdownExtraction, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
                QObject::connect(sShutdownExtraction, &QState::entered, extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::Shutdown);
                sShutdownExtraction->addTransition(extraction_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, sShutdownFilament);

                QObject::connect(sShutdownFilament, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
                QObject::connect(sShutdownFilament, &QState::entered, filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::Shutdown);
                sShutdownFilament->addTransition(filament_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, sShutdownArc);

                QObject::connect(sShutdownArc, &global::TimedState::SIGNAL_Timeout, this, &Source::ErrorMessage);
                QObject::connect(sShutdownArc, &QState::entered, arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::Shutdown);
                auto tShutdownFinished = sShutdownArc->addTransition(arc_, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, sIdle);
                QObject::connect(tShutdownFinished, &QSignalTransition::triggered, this, [&]() {
                    InfoMessage("Arrêt terminé");
                    emit SIGNAL_ShutdownFinished();
                });

     PrintStateChanges(sSuperState, "SuperState");
     PrintStateChanges(sIdle, "Idle");
     PrintStateChanges(sConfigure, "Configure");
     PrintStateChanges(sStartup, "Startup");
     PrintStateChanges(sStartupArc, "StartupArc");
     PrintStateChanges(sStartupFilament, "StartupFilament");
     PrintStateChanges(sStartupExtraction, "StartupExtraction");
//     PrintStateChanges(sStartupInjection, "StartupInjection");
     PrintStateChanges(sShutdown, "Shutdown");
     PrintStateChanges(sShutdownInjection, "ShutdownInjection");
     PrintStateChanges(sShutdownExtraction, "ShutdownExtraction");
     PrintStateChanges(sShutdownFilament, "ShutdownFilament");
     PrintStateChanges(sShutdownArc, "ShutdownArc");

     sm_.addState(sSuperState);
     sm_.setInitialState(sSuperState);
     sm_.start();
}

void Source::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "SOURCE ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() <<  "SOURCE <-" << name; });
}

void Source::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Source"), message);
}

void Source::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Source"), message);
}

void Source::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Source"), message);
}

void Source::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Source"), message);
}

}
