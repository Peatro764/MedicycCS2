#include "Membrane.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

Membrane::Membrane(HFX hfx) :
    hfx_(hfx),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.hf", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    ConnectSignals();
    SetupStateMachine();
    qDebug() << "Membrane::Membrane DBUS "
             << " Omron " << omron_.isValid()
             << " ADC " << adc_.isValid()
             << " Logger " << logger_.isValid();

    ping_timer_.setInterval(ping_slow_interval_);
    ping_timer_.setSingleShot(false);
    ping_timer_.start();
    QObject::connect(&ping_timer_, &QTimer::timeout, this, &Membrane::Ping);
}

Membrane::~Membrane() {

}

void Membrane::Configure_UpperLimit(double value) {
    upper_limit_ = value;
    qDebug() << "Membrane::Configure_UpperLimit " << value;
}

void Membrane::Configure_LowerLimit(double value) {
    lower_limit_ = value;
    qDebug() << "Membrane::Configure_LowerLimit " << value;
}

void Membrane::Configure_LimitClearance(double value) {
    limit_clearance_ = value;
    qDebug() << "Membrane::Configure_LimitClearance " << value;
}

void Membrane::SetFastPing() {
    ping_timer_.setInterval(ping_fast_interval_);
}

void Membrane::SetSlowPing() {
    ping_timer_.setInterval(ping_slow_interval_);
}

void Membrane::SetModeAutomatic() {
    emit SIGNAL_SetModeAutomatic();
}

void Membrane::SetModeManual() {
    emit SIGNAL_SetModeManual();
}

void Membrane::MoveToAccordPosition() {
    if (current_position_ > (accord_position_ + 3.0)) {
        emit SIGNAL_StartMovingDown();
    } else if (current_position_ < (accord_position_ - 3.0)) {
        emit SIGNAL_StartMovingUp();
    } else {
        emit SIGNAL_AccordPositionReached();
    }
}

void Membrane::StartMovingUp() {
    emit SIGNAL_StartMovingUp();
}

void Membrane::StartMovingDown() {
    emit SIGNAL_StartMovingDown();
}

void Membrane::Interrupt() {
    Stop();
}

void Membrane::Stop() {
    emit SIGNAL_Stop();
}

void Membrane::ReadIsMovingUp() {
    omron_.ReadChannel(CmdStringMoveUp());
}

void Membrane::ReadIsMovingDown() {
    omron_.ReadChannel(CmdStringMoveDown());
}

void Membrane::ReadPosition() {
    adc_.Read(CmdStringReadPosition());
}

void Membrane::ReadMode() {
    omron_.ReadChannel(CmdStringReadMode());
}

void Membrane::ReadLimitSwitch() {
    omron_.ReadChannel(CmdStringReadLimitSwitches());
}

void Membrane::ConnectSignals() {
    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead , this, &Membrane::InterpretOmronBitRead);
    QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, &Membrane::InterpretADCChannelRead);

    QObject::connect(this, &Membrane::SIGNAL_Stopped, this, [&]() { DebugMessage("Membrane arrêté"); });
}

void Membrane::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sDisconnected = new QState(sSuperState);
        QState *sConnected = new QState(sSuperState);
            global::TimedState *sCheckMode = new global::TimedState(sConnected, 3000, "Demander (encore) quel mode (auto/manu) est actif");
            QState *sAutomaticMode = GetModeAutoState(sConnected);
            QState *sManualMode = GetModeManualState(sConnected);

            // SuperState
            sSuperState->setInitialState(sDisconnected);

                // Disconnected
                auto tConnected = sDisconnected->addTransition(this, &Membrane::SIGNAL_Connected, sConnected);
                QObject::connect(tConnected, &QSignalTransition::triggered, this, [&]() { DebugMessage("Connexion omron et adc établie"); });

                // Connected
                auto tDisconnected = sConnected->addTransition(this, &Membrane::SIGNAL_Disconnected, sDisconnected);
                QObject::connect(tDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error(); });
                sConnected->setInitialState(sCheckMode);

                    // CheckMode
                    QObject::connect(sCheckMode, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
                    QObject::connect(sCheckMode, &QState::entered, this, &Membrane::ReadMode);
                    sCheckMode->addTransition(this, &Membrane::SIGNAL_Manual, sManualMode);
                    sCheckMode->addTransition(this, &Membrane::SIGNAL_Automatic, sAutomaticMode);

                    // Automatic mode
                    QObject::connect(sAutomaticMode, &global::TimedState::entered, this, [&]() { DebugMessage("Membrane mode automatique"); });
                    sAutomaticMode->addTransition(this, &Membrane::SIGNAL_Manual, sManualMode);

                    // Manual mode
                    QObject::connect(sManualMode, &global::TimedState::entered, this, [&]() { DebugMessage("Membrane mode manuel"); });
                    sManualMode->addTransition(this, &Membrane::SIGNAL_Automatic, sAutomaticMode);

            PrintStateChanges(sSuperState, "SuperState");
            PrintStateChanges(sDisconnected, "Disconnected");
            PrintStateChanges(sConnected, "Connected");
            PrintStateChanges(sCheckMode, "CheckMode");
            PrintStateChanges(sAutomaticMode, "AutomaticMode");
            PrintStateChanges(sManualMode, "ManualMode");

            sm_.addState(sSuperState);
            sm_.setInitialState(sSuperState);
            sm_.start();
}

QState* Membrane::GetModeAutoState(QState *parent) {
    QState *sAutomaticMode = new QState(parent);
        QState *sIdle = new QState(sAutomaticMode);
        QState *sError = new QState(sAutomaticMode);
        // An operator could manually push the auto button while the membrane is commanded up or down. Thus
        // always stop all manual commands when entering in mode auto
        global::TimedState *sStopManualCommands = new global::TimedState(sAutomaticMode, 5000, "Echec de la commande d'arrêt de la membrane", sError);
            global::TimedState *sWriteStopManualUp = new global::TimedState(sStopManualCommands, 1000, "Réessayer la commande arrêter de monter la membrane");
            global::TimedState *sWriteStopManualDown = new global::TimedState(sStopManualCommands, 1000, "Réessayer la commande arrêter de descendre la membrane");
        global::TimedState *sWriteModeManuel = new global::TimedState(sAutomaticMode, 6000, "Nombre maximum d'essais d'écriture mode manuel de la membraneatteint", sError);
            global::TimedState *sWriteModeManuel_FlankUp = new global::TimedState(sWriteModeManuel, 1000, "Réessayer la commande mode manuel de la membrane flanc montant");
            global::TimedState *sWriteModeManuel_FlankDown = new global::TimedState(sWriteModeManuel, 1000, "Réessayer la commande mode manuel de la membrane flanc descendent");
        global::TimedState *sVerifyModeManuel = new global::TimedState(sAutomaticMode, 5000, "Echec de la vérification si le mode manuel de la membrane est activé", sError);

        // AutomaticMode
        sAutomaticMode->setInitialState(sStopManualCommands);

            // Error
            QState::connect(sError, &QState::entered, this, &Membrane::SIGNAL_Error);
            sError->addTransition(sError, &QState::entered, sIdle);

            // Idle
            sIdle->addTransition(this, &Membrane::SIGNAL_SetModeManual, sWriteModeManuel);

            // StopManualCommands
            sStopManualCommands->setInitialState(sWriteStopManualUp);
            // Swallow this signal. Dont want to transition out before flank down is done
            sStopManualCommands->addTransition(new QSignalTransition(this, &Membrane::SIGNAL_Manual));
            QObject::connect(sStopManualCommands, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);

                // WriteStopManualUp
                QObject::connect(sWriteStopManualUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
                QObject::connect(sWriteStopManualUp, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringMoveUp(), false);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveUp()); });
                });
                sWriteStopManualUp->addTransition(this, &Membrane::SIGNAL_NotMovingUp, sWriteStopManualDown);

                // WriteStopManualDown
                QObject::connect(sWriteStopManualDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
                QObject::connect(sWriteStopManualDown, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringMoveDown(), false);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveDown()); });
                });
                auto tStoppedInAuto = sWriteStopManualDown->addTransition(this, &Membrane::SIGNAL_NotMovingDown, sIdle);
                QObject::connect(tStoppedInAuto, &QSignalTransition::triggered, this, &Membrane::SIGNAL_Stopped);

            // WriteModeManuel
            sWriteModeManuel->setInitialState(sWriteModeManuel_FlankUp);
            // Swallow this signal. Dont want to transition out before flank down is done
            sWriteModeManuel->addTransition(new QSignalTransition(this, &Membrane::SIGNAL_Manual));
            QObject::connect(sWriteModeManuel, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);

                // WriteModeManuel_FlankUp
                QObject::connect(sWriteModeManuel_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
                QObject::connect(sWriteModeManuel_FlankUp, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringSetModeManual(), true);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringSetModeManual()); });
                });
                sWriteModeManuel_FlankUp->addTransition(this, &Membrane::SIGNAL_WriteModeManu_True, sWriteModeManuel_FlankDown);

                // WriteModeManuel_FlankDown
                QObject::connect(sWriteModeManuel_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
                QObject::connect(sWriteModeManuel_FlankDown, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringSetModeManual(), false);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringSetModeManual()); });
                });
                auto tWriteModeManuel_Ok = sWriteModeManuel_FlankDown->addTransition(this, &Membrane::SIGNAL_WriteModeManu_False , sVerifyModeManuel);
                QObject::connect(tWriteModeManuel_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion mode manuel écrite"); });

            // VerifyModeManuel
            QObject::connect(sVerifyModeManuel, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);
            QObject::connect(sVerifyModeManuel, &QState::entered, this, &Membrane::ReadMode);

            PrintStateChanges(sIdle, "Idle");
            PrintStateChanges(sStopManualCommands, "StopManualCommands");
            PrintStateChanges(sWriteStopManualUp, "WriteStopManualUp");
            PrintStateChanges(sWriteStopManualDown, "WriteStopManualDown");
            PrintStateChanges(sWriteModeManuel, "WriteModeManuel");
            PrintStateChanges(sWriteModeManuel_FlankUp, "WriteModeManuel_FlankUp");
            PrintStateChanges(sWriteModeManuel_FlankDown, "WriteModeManuel_FlankDown");
            PrintStateChanges(sVerifyModeManuel, "VerifyModeManuel");

            return sAutomaticMode;
}

QState* Membrane::GetModeManualState(QState *parent) {
    QState *sManualMode = new QState(parent);
        QState *sError = new QState(sManualMode);
        QState *sIdle = new QState(sManualMode);
        global::TimedState *sStopping = new global::TimedState(sManualMode, 10000, "Echec de la commande d'arrêt de la membrane", sError);
            global::TimedState *sWriteStopUp = new global::TimedState(sStopping, 2000, "Réessayer la commande arrêter de monter la membrane");
            global::TimedState *sWriteStopDown = new global::TimedState(sStopping, 2000, "Réessayer la commande arrêter de descendre la membrane");
        global::TimedState *sCommandMovingDown = new global::TimedState(sManualMode, 15000, "Echec commande descendre", sError);
            global::TimedState *sWriteStopUpBeforeMovingDown = new global::TimedState(sCommandMovingDown, 2000, "Réessayer la commande arrêter de monter la membrane");
            global::TimedState *sWriteMoveDown = new global::TimedState(sCommandMovingDown, 2000, "Réessayer la commande descendre la membrane");
        QState *sMovingDown = new QState(sManualMode);
        global::TimedState *sCommandMovingUp = new global::TimedState(sManualMode, 15000, "Echec commande monter", sError);
            global::TimedState *sWriteStopDownBeforeMovingUp = new global::TimedState(sCommandMovingUp, 2000, "Réessayer la commande arrêter de descendre la membrane");
            global::TimedState *sWriteMoveUp = new global::TimedState(sCommandMovingUp, 2000, "Réessayer la commande monter la membrane");
        QState *sMovingUp = new QState(sManualMode);
        global::TimedState *sWriteModeAuto = new global::TimedState(sManualMode, 10000, "Nombre maximum d'essais d'écriture mode auto de la membrane atteint", sError);
             global::TimedState *sWriteModeAuto_FlankUp = new global::TimedState(sWriteModeAuto, 2000, "Réessayer la commande mode auto de la membrane flanc montant");
             global::TimedState *sWriteModeAuto_FlankDown = new global::TimedState(sWriteModeAuto, 2000, "Réessayer la commande mode auto de la membrane flanc descendent");
        global::TimedState *sVerifyModeAuto = new global::TimedState(sManualMode, 10000, "Echec de la vérification si le mode auto de la membrane est activé", sError);

         sManualMode->addTransition(this, &Membrane::SIGNAL_Stop, sStopping)->setTransitionType(QAbstractTransition::InternalTransition);
         sManualMode->setInitialState(sStopping);

         // Error
         QState::connect(sError, &QState::entered, this, &Membrane::SIGNAL_Error);
         sError->addTransition(sError, &QState::entered, sIdle);

         // IdleManual
         sIdle->addTransition(this, &Membrane::SIGNAL_SetModeAutomatic, sWriteModeAuto);
         sIdle->addTransition(this, &Membrane::SIGNAL_StartMovingDown, sCommandMovingDown);
         sIdle->addTransition(this, &Membrane::SIGNAL_StartMovingUp, sCommandMovingUp);

         // Stopping
         sStopping->setInitialState(sWriteStopUp);
         QObject::connect(sStopping, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);
         sStopping->addTransition(new QSignalTransition(this, &Membrane::SIGNAL_Automatic)); // swallow

             // WriteStopUp
             QObject::connect(sWriteStopUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteStopUp, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveUp(), false);
                QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveUp()); });
             });
             sWriteStopUp->addTransition(this, &Membrane::SIGNAL_NotMovingUp, sWriteStopDown);

             // WriteStopDown
             QObject::connect(sWriteStopDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteStopDown, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveDown(), false);
                 QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveDown()); });
             });
             auto tStoppedInManual = sWriteStopDown->addTransition(this, &Membrane::SIGNAL_NotMovingDown, sIdle);
             QObject::connect(tStoppedInManual, &QSignalTransition::triggered, this, &Membrane::SIGNAL_Stopped);

         // CommandMovingDown
         QObject::connect(sCommandMovingDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);
         sCommandMovingDown->setInitialState(sWriteStopUpBeforeMovingDown);

             // WriteStopUpBeforeMovingDown
             QObject::connect(sWriteStopUpBeforeMovingDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteStopUpBeforeMovingDown, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveUp(), false);
                 QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveUp()); });
             });
             sWriteStopUpBeforeMovingDown->addTransition(this, &Membrane::SIGNAL_NotMovingUp, sWriteMoveDown);

             // WriteMoveDown
             QObject::connect(sWriteMoveDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteMoveDown, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveDown(), true);
                 QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveDown()); });
             });
             sWriteMoveDown->addTransition(this, &Membrane::SIGNAL_MovingDown, sMovingDown);

         // MovingDown
         QObject::connect(sMovingDown, &global::TimedState::entered, this, [&]() { DebugMessage("Descendre membrane"); });
         QObject::connect(sMovingDown, &global::TimedState::entered, this, &Membrane::SetFastPing);
         QObject::connect(sMovingDown, &global::TimedState::exited, this, &Membrane::SetSlowPing);
         sMovingDown->addTransition(this, &Membrane::SIGNAL_StartMovingUp, sCommandMovingUp);
         auto tLimitSwitchWhenMovingDown = sMovingDown->addTransition(this, &Membrane::SIGNAL_HWLimitReached, sStopping);
         QObject::connect(tLimitSwitchWhenMovingDown, &QSignalTransition::triggered, this, [&](){
             ErrorMessage("Fin de course déclenché");
             emit SIGNAL_Error();
         });
         auto tSWLimitDown = sMovingDown->addTransition(this, &Membrane::SIGNAL_SWLowerLimitReached, sStopping);
         QObject::connect(tSWLimitDown, &QSignalTransition::triggered, this, [&](){ InfoMessage("SW Fin de course"); });

         // CommandMovingUp
         QObject::connect(sCommandMovingUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);
         sCommandMovingUp->setInitialState(sWriteStopDownBeforeMovingUp);

             // WriteStopDownBeforeMovingUp
             QObject::connect(sWriteStopDownBeforeMovingUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteStopDownBeforeMovingUp, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveDown(), false);
                 QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveDown()); });
             });
             sWriteStopDownBeforeMovingUp->addTransition(this, &Membrane::SIGNAL_NotMovingDown, sWriteMoveUp);

             // WriteMoveUp
             QObject::connect(sWriteMoveUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
             QObject::connect(sWriteMoveUp, &QState::entered, this, [&]() {
                 omron_.WriteChannel(CmdStringMoveUp(), true);
                QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringMoveUp()); });
             });
             sWriteMoveUp->addTransition(this, &Membrane::SIGNAL_MovingUp, sMovingUp);

        // MovingUp
        QObject::connect(sMovingUp, &global::TimedState::entered, this, [&]() { DebugMessage("Monter membrane"); });
        QObject::connect(sMovingUp, &global::TimedState::entered, this, &Membrane::SetFastPing);
        QObject::connect(sMovingUp, &global::TimedState::exited, this, &Membrane::SetSlowPing);
        sMovingUp->addTransition(this, &Membrane::SIGNAL_StartMovingDown, sCommandMovingDown);
        auto tLimitSwitchWhenMovingUp = sMovingUp->addTransition(this, &Membrane::SIGNAL_HWLimitReached, sStopping);
        QObject::connect(tLimitSwitchWhenMovingUp, &QSignalTransition::triggered, this, [&](){
            ErrorMessage("Fin de course déclenché");
            emit SIGNAL_Error();
        });
        auto tSWLimitUp = sMovingUp->addTransition(this, &Membrane::SIGNAL_SWUpperLimitReached, sStopping);
        QObject::connect(tSWLimitUp, &QSignalTransition::triggered, this, [&](){ WarningMessage("SW Fin de course haut de la membrane"); });

        // WriteModeAuto
        sWriteModeAuto->setInitialState(sWriteModeAuto_FlankUp);
        sWriteModeAuto->addTransition(new QSignalTransition(this, &Membrane::SIGNAL_Automatic)); // swallow
        sWriteModeAuto->addTransition(new QSignalTransition(this, &Membrane::SIGNAL_Stop)); // swallow
        QObject::connect(sWriteModeAuto, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);

            // WriteModeAuto_FlankUp
            QObject::connect(sWriteModeAuto_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
            QObject::connect(sWriteModeAuto_FlankUp, &QState::entered, this, [&]() {
                omron_.WriteChannel(CmdStringSetModeAutomatic(), true);
                QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringSetModeAutomatic()); });
            });
            sWriteModeAuto_FlankUp->addTransition(this, &Membrane::SIGNAL_WriteModeAuto_True, sWriteModeAuto_FlankDown);

            // WriteModeAuto_FlankDown
            QObject::connect(sWriteModeAuto_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Membrane::WarningMessage);
            QObject::connect(sWriteModeAuto_FlankDown, &QState::entered, this, [&]() {
                omron_.WriteChannel(CmdStringSetModeAutomatic(), false);
                QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringSetModeAutomatic()); });
            });
            auto tWriteModeAuto_Ok = sWriteModeAuto_FlankDown->addTransition(this, &Membrane::SIGNAL_WriteModeAuto_False , sVerifyModeAuto);
            QObject::connect(tWriteModeAuto_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion mode auto écrite de la membrane"); });

        // VerifyModeAuto
        QObject::connect(sVerifyModeAuto, &global::TimedState::SIGNAL_Timeout, this, &Membrane::ErrorMessage);
        QObject::connect(sVerifyModeAuto, &QState::entered, this, &Membrane::ReadMode);

     PrintStateChanges(sManualMode, "ManualMode");
     PrintStateChanges(sError, "Error");
     PrintStateChanges(sIdle, "IdleManual");
     PrintStateChanges(sStopping, "Stopping");
     PrintStateChanges(sWriteStopUp, "WriteStopUp");
     PrintStateChanges(sWriteStopDown, "WriteStopDown");
     PrintStateChanges(sCommandMovingDown, "CommandMovingDown");
     PrintStateChanges(sWriteStopUpBeforeMovingDown, "WriteStopUpBeforeMovingDown");
     PrintStateChanges(sWriteMoveDown, "WriteMoveDown");
     PrintStateChanges(sMovingDown, "MovingDown");
     PrintStateChanges(sCommandMovingUp, "CommandMovingUp");
     PrintStateChanges(sWriteStopDownBeforeMovingUp, "WriteStopDownBeforeMovingUp");
     PrintStateChanges(sWriteMoveUp, "WriteMoveUp");
     PrintStateChanges(sMovingUp, "MovingUp");
     PrintStateChanges(sWriteModeAuto, "WriteModeAuto");
     PrintStateChanges(sWriteModeAuto_FlankDown, "WriteModeAuto_FlankDown");
     PrintStateChanges(sWriteModeAuto_FlankUp, "WriteModeAuto_FlankUp");
     PrintStateChanges(sVerifyModeAuto, "VerifyModeAuto");

     return sManualMode;
}

void Membrane::Ping() {
    qDebug() << QDateTime::currentDateTime() << " Membrane::Ping";
    ReadMode();
    ReadPosition();
    ReadLimitSwitch();
    (omron_.isValid() && adc_.isValid()) ? emit SIGNAL_Connected() : emit SIGNAL_Disconnected();
}

void Membrane::SaveAccordPosition() {
    // Save a value slightly below so that the membrane can start moving in the
    // same direction (up) during startup and rapidly reach the position. Keep a small
    // margin however to make sure that daily changes to the HF does not impact the procedure.
    double pos = std::max(current_position_ - 10.0, lower_limit_);
    pos = std::min(pos, upper_limit_);
    accord_position_ = pos;
}

void Membrane::PositionReceived(double value) {
    current_position_ = value;
    if (std::abs(current_position_- accord_position_) < 2.0) emit SIGNAL_AccordPositionReached();
    (value >= upper_limit_) ? emit SIGNAL_SWUpperLimitReached() : emit SIGNAL_SWUpperLimitNotReached();
    (value <= lower_limit_) ? emit SIGNAL_SWLowerLimitReached() : emit SIGNAL_SWLowerLimitNotReached();
    if (value >= (lower_limit_ + limit_clearance_) && value <= (upper_limit_ - limit_clearance_)) {
        emit SIGNAL_ClearedFromLimits();
    }
    emit SIGNAL_Position(value);
}

void Membrane::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Membrane " + GetHFBranch() + " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Membrane " + GetHFBranch() +" <-" << name; });
}

void Membrane::InterpretADCChannelRead(const QString& channel, double value) {
    if (channel == CmdStringReadPosition()) {
        PositionReceived(value);
    } // else no action
}

void Membrane::InterpretOmronBitRead(const QString& channel, bool content) {
    if (channel == CmdStringSetModeAutomatic()) {
        content ? emit  SIGNAL_WriteModeAuto_True() : emit SIGNAL_WriteModeAuto_False();
    } else if (channel == CmdStringSetModeManual()) {
        content ? emit SIGNAL_WriteModeManu_True() : emit SIGNAL_WriteModeManu_False();
    } else if (channel == CmdStringMoveUp()) {
        content ? emit SIGNAL_MovingUp() : emit SIGNAL_NotMovingUp();
    } else if(channel == CmdStringMoveDown()) {
        content ? emit SIGNAL_MovingDown() : emit SIGNAL_NotMovingDown();
    } else if (channel == CmdStringReadMode()) {
        content ? emit SIGNAL_Automatic() : emit SIGNAL_Manual();
    } else if (channel == CmdStringReadLimitSwitches()) {
        content ? emit SIGNAL_HWLimitReached() : emit SIGNAL_HWLimitNotReached();
    }
//    else {
//        qDebug() << "Unknown omron channel " << channel;
//    }
}

QString Membrane::GetHFBranch() const {
    return QString("HF%1").arg(static_cast<int>(hfx_));
}

void Membrane::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Membrane::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Membrane::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Membrane::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

}
