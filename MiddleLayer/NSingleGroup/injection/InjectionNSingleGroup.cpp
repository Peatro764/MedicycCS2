#include "InjectionNSingleGroup.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>
#include <QSignalTransition>

#include "TimedState.h"
#include "CountedTimedState.h"
#include "Enumerations.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::injection  {

InjectionNSingleGroup::InjectionNSingleGroup(int iteration_delay) :
    iteration_delay_(iteration_delay),
    repo_("_prod"),
    nsingle_group_("INJECTION", iteration_delay),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "InjectionNSingleGroup";
    SetupSteerer();
    ConnectSignals();
    SetupStateMachine();
}

InjectionNSingleGroup::~InjectionNSingleGroup() {
    if (steerer_nsingle_) {
        delete steerer_nsingle_;
    }
}


void InjectionNSingleGroup::ConnectSignals() {
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_Configuration, this, &InjectionNSingleGroup::SIGNAL_Configuration);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_State_Ready,this, &InjectionNSingleGroup::SIGNAL_State_Ready);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_State_Off,this, &InjectionNSingleGroup::SIGNAL_State_Off);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_State_Unknown,this, &InjectionNSingleGroup::SIGNAL_State_Unknown);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_State_Intermediate,this, &InjectionNSingleGroup::SIGNAL_State_Intermediate);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_StartupFinished,this, &InjectionNSingleGroup::SIGNAL_StartupFinished);
    QObject::connect(&nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::SIGNAL_ShutdownFinished,this, &InjectionNSingleGroup::SIGNAL_ShutdownFinished);
}

void InjectionNSingleGroup::SetupSteerer() {
    try {
        nsingle::NSingleDbusAddress dbus = repo_.GetNSingleDbusAddress("Multiplexed_2");
        steerer_nsingle_ = new medicyc::cyclotron::NSingleInterface(dbus.interface_address(), dbus.nsingle_object_name(),
                                                                    medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
    } catch(std::exception& exc) {
        qWarning() << "InjectionNSingleGroup::SetupSteererNSingle Exception caught: " << exc.what();
        ErrorMessage(QString("Echec de la configuration du nsingle steerer: ") +exc.what());
    }
}

void InjectionNSingleGroup::Configure(QString cyclo_config) {
    nsingle_group_.Configure(cyclo_config);
}

void InjectionNSingleGroup::QueryConfiguration() {
    nsingle_group_.QueryConfiguration();
}

void InjectionNSingleGroup::Startup() {
    emit SIGNAL_Startup();
}

void InjectionNSingleGroup::Shutdown() {
    emit SIGNAL_Shutdown();
}

void InjectionNSingleGroup::Interrupt() {
    nsingle_group_.Interrupt();
    emit SIGNAL_Interrupt();
}

void InjectionNSingleGroup::SetupStateMachine() {
    qDebug() << "InjectionNSingleGroup::SetupStateMachine";
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        global::CountedTimedState *sSwitchOnSteerer = new global::CountedTimedState(sSuperState, 3, "Echec de démarrage de l'équipement", 3000, "Alimentation toujours étaint");
        global::TimedState *sStartupNSingleGroup = new global::TimedState(sSuperState, 60000, "Wait", sIdle);
        global::CountedTimedState *sSwitchOffSteerer = new global::CountedTimedState(sSuperState, 3, "Echec de l'arrêt de l'équipement", 3000, "Alimentation toujours en marche");
        global::TimedState *sShutdownNSingleGroup = new global::TimedState(sSuperState, 60000, "Wait", sSwitchOffSteerer);

    // SuperState
    sSuperState->setInitialState(sIdle);
    sSuperState->addTransition(this, &InjectionNSingleGroup::SIGNAL_Startup, sSwitchOnSteerer);
    sSuperState->addTransition(this, &InjectionNSingleGroup::SIGNAL_Shutdown, sShutdownNSingleGroup);
    sSuperState->addTransition(this, &InjectionNSingleGroup::SIGNAL_Interrupt, sIdle);

    // Idle
    // no further transitions

    // SwitchOnSteerer
    QObject::connect(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_Timeout, this, &InjectionNSingleGroup::WarningMessage);
    QObject::connect(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, this, &InjectionNSingleGroup::ErrorMessage);
    QObject::connect(sSwitchOnSteerer, &QState::entered, steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::On);
    QObject::connect(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, sSwitchOnSteerer, &global::CountedTimedState::Reset);
    QObject::connect(steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::SIGNAL_On, sSwitchOnSteerer, &global::CountedTimedState::Reset);
    QObject::connect(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_Timeout, sSwitchOnSteerer, &global::CountedTimedState::Increment);
    sSwitchOnSteerer->addTransition(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_Timeout, sSwitchOnSteerer);
    sSwitchOnSteerer->addTransition(sSwitchOnSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, sIdle);
    sSwitchOnSteerer->addTransition(steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::SIGNAL_On, sStartupNSingleGroup);

    // StartupNSingleGroup
    QObject::connect(sStartupNSingleGroup, &QState::entered, &nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::Startup);
    sStartupNSingleGroup->addTransition(sStartupNSingleGroup, &QState::entered, sIdle);

    // ShutdownNSingleGroup
    QObject::connect(sShutdownNSingleGroup, &QState::entered, &nsingle_group_, &medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup::Shutdown);

    // SwitchOffSteerer
    QObject::connect(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_Timeout, this, &InjectionNSingleGroup::WarningMessage);
    QObject::connect(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, this, &InjectionNSingleGroup::ErrorMessage);
    QObject::connect(sSwitchOffSteerer, &QState::entered, steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::Off);
    QObject::connect(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, sSwitchOffSteerer, &global::CountedTimedState::Reset);
    QObject::connect(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_Timeout, sSwitchOffSteerer, &global::CountedTimedState::Increment);
    QObject::connect(steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::SIGNAL_Off, sSwitchOffSteerer, &global::CountedTimedState::Reset);
    QObject::connect(steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::SIGNAL_Off, this, [&]() { InfoMessage("Steerers injections arretes"); });
    sSwitchOffSteerer->addTransition(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_Timeout, sSwitchOffSteerer);
    sSwitchOffSteerer->addTransition(sSwitchOffSteerer, &global::CountedTimedState::SIGNAL_ThresholdReached, sIdle);
    sSwitchOffSteerer->addTransition(steerer_nsingle_, &medicyc::cyclotron::NSingleInterface::SIGNAL_Off, sIdle);

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sSwitchOnSteerer, "SwitchOnSteerer");
    PrintStateChanges(sStartupNSingleGroup, "StartupNSingleGroup");
    PrintStateChanges(sShutdownNSingleGroup, "ShutdownNSingleGroup");
    PrintStateChanges(sSwitchOffSteerer, "SwitchOffSteerer");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}


void InjectionNSingleGroup::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("InjectionNSingleGroup"), message);
}

void InjectionNSingleGroup::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("InjectionNSingleGroup"), message);
}

void InjectionNSingleGroup::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("InjectionNSingleGroup"), message);
}

void InjectionNSingleGroup::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "InjectionNSingleGroup " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "InjectionNSingleGroup " << QDateTime::currentDateTime() << " <-" << name; });
}

}
