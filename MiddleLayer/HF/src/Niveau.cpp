#include "Niveau.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

Niveau::Niveau(HFX hfx) :
    hfx_(hfx),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.hf", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())

{
    ConnectSignals();
    SetupStateMachine();
    qDebug() << "Niveau::Niveau DBUS "
             << " Omron " << omron_.isValid()
             << " ADC " << adc_.isValid()
             << " Logger " << logger_.isValid();
}

Niveau::~Niveau() {

}

// Public

void Niveau::Configure_NiveauBeforeOn(double value) {
    level_before_on_ = value;
    qDebug() << "Niveau::Configure_NiveauBeforeOn " << value;
}

void Niveau::Configure_NiveauBeforeOff(double value) {
    level_before_off_ = value;
    qDebug() << "Niveau::Configure_NiveauBeforeOff " << value;
}

void Niveau::Configure_NiveauStartupFinalValue(double value) {
    level_startup_final_ = value;
    qDebug() << "Niveau::Configure_NiveauStartupFinalValue " << value;
}

void Niveau::Configure_NiveauShutdownInitialValue(double value) {
    level_shutdown_initial_ = value;
    qDebug() << "Niveau::Configure_NiveauShutdownInitialValue " << value;
}

void Niveau::Configure_NiveauMax(double value) {
    level_max_ = value;
    qDebug() << "Niveau::Configure_NiveauMax " << value;
}

void Niveau::On() {
    qDebug() << "Niveau::On";
    emit SIGNAL_CmdOn();
}

void Niveau::Off() {
    qDebug() << "Niveau::Off";
    emit SIGNAL_CmdOff();
}

void Niveau::Increment() {
    qDebug() << "Niveau::Increment";
    if (level_act_ < level_max_) {
        emit SIGNAL_CmdIncrement();
    } else {
        WarningMessage("Niveau max atteint");
        emit SIGNAL_Error();
    }
}

void Niveau::Decrement() {
    qDebug() << "Niveau::Decrement";
    emit SIGNAL_CmdDecrement();
}

void Niveau::SetStartupFinalLevel() {
    qDebug() << "Niveau::SetStartupFinalLevel";
    emit SIGNAL_SetStartupFinalValue();
}

void Niveau::SetShutdownInitialLevel() {
    qDebug() << "Niveau::SetShutdownInitialLevel";
    emit SIGNAL_SetShutdownInitialValue();
}

// TODO remove?
void Niveau::SetLevel(double value) {
    if (value < level_max_) {
        level_set_ = value;
        emit SIGNAL_CmdSetLevel();
    } else {
        ErrorMessage("Niveau non autorisé (>" + QString::number(level_max_) + ")");
        emit SIGNAL_Error();
    }
}

void Niveau::Interrupt() {
    emit SIGNAL_Interrupt();
}

// Private

void Niveau::IsAllowedOn() {
    omron_.ReadChannel(CmdStringIsAllowedOn());
}

void Niveau::IsOn() {
    omron_.ReadChannel(CmdStringIsOn());
}

void Niveau::ReadValue() {
    adc_.Read(CmdStringReadValue());
}

QString Niveau::GetHFBranch() const {
    return QString("HF%1").arg(static_cast<int>(hfx_));
}

void Niveau::Ping() {
    qDebug() << QDateTime::currentDateTime() << " Niveau::Ping";
    IsOn();
    IsAllowedOn();
    ReadValue();
    (omron_.isValid() && adc_.isValid()) ? emit SIGNAL_Connected() : emit SIGNAL_Disconnected();
}

void Niveau::SetupStateMachine() {

    QState *sSuperState = new QState();
        QState *sDisconnected = new QState(sSuperState);
        QState *sConnected = new QState(sSuperState);
            QState *sOn = new QState(sConnected);
            QState *sOff = new QState(sConnected);

        // SuperState
        sSuperState->setInitialState(sDisconnected);
        QTimer *tPing = new QTimer(sSuperState);
        tPing->setInterval(ping_interval_);
        tPing->setSingleShot(false);
        tPing->start();
        QObject::connect(tPing, &QTimer::timeout, this, &Niveau::Ping);

            // Disconnected
            auto tConnected = sDisconnected->addTransition(this, &Niveau::SIGNAL_Connected, sConnected);
            QObject::connect(tConnected, &QSignalTransition::triggered, this, [&]() { DebugMessage("Connexion omron et adc établie"); });

            // Connected
            auto tDisconnected = sConnected->addTransition(this, &Niveau::SIGNAL_Disconnected, sDisconnected);
            QObject::connect(tDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error(); });
            sConnected->setInitialState(sOff);
                // Off
                CreateOffState(sOff);
                auto tOn = sOff->addTransition(this, &Niveau::SIGNAL_On, sOn);
                QObject::connect(tOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau on"); });

                // On
                CreateOnState(sOn);
                auto tOff = sOn->addTransition(this, &Niveau::SIGNAL_Off, sOff);
                QObject::connect(tOff, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau off"); });

             PrintStateChanges(sSuperState, "SuperState");
             PrintStateChanges(sDisconnected, "Disconnected");
             PrintStateChanges(sConnected, "Connected");
             PrintStateChanges(sOn, "On");
             PrintStateChanges(sOff, "Off");

             sm_.addState(sSuperState);
             sm_.setInitialState(sSuperState);
             sm_.start();
}

void Niveau::CreateOffState(QState *parent) {
    QState *sError = new QState(parent);
    QState *sIdle = new QState(parent);
    QState *sHalt = new QState(parent);
    global::TimedState *sInc = new global::TimedState(parent, 3000, "Timeout incrément", sError);
    global::TimedState *sDec = new global::TimedState(parent, 3000, "Timeout décrement", sError);
    global::TimedState *sSetLevel = new global::TimedState(parent, 60000, "Echec du réglage du niveau", sError);
    global::TimedState *sCommandOn = new global::TimedState(parent, 30000, "Délai d'attente pour mettre niveau on", sError);
        global::TimedState *sVerifyOnAllowed = new global::TimedState(sCommandOn, 3000, "Echec de la vérification si le niveau on est autorisé");
        global::TimedState *sSetPreOnLevel = new global::TimedState(sCommandOn, 30000, "Echec du réglage du niveau", sError);
        global::TimedState *sWriteOn = new global::TimedState(sCommandOn, 3000, "Nombre maximum d'essais de niveau on atteint", sError);
            global::TimedState *sWriteOn_FlankUp = new global::TimedState(sWriteOn, 1000, "Réessayer la commande on niveau flanc montant");
            global::TimedState *sWriteOn_FlankDown = new global::TimedState(sWriteOn, 1000, "Réessayer la commande on niveau flanc descendent");
        global::TimedState *sVerifyOn = new global::TimedState(sCommandOn, 5000, "Echec de la vérification si le niveau on", sError);

        // parent
        parent->setInitialState(sIdle);
        parent->addTransition(this, &Niveau::SIGNAL_CmdOn, sCommandOn);
        parent->addTransition(this, &Niveau::SIGNAL_CmdSetLevel, sSetLevel)->setTransitionType(QAbstractTransition::InternalTransition);;
        parent->addTransition(this, &Niveau::SIGNAL_Interrupt, sHalt)->setTransitionType(QAbstractTransition::InternalTransition);;

        // Error
        QState::connect(sError, &QState::entered, this, &Niveau::SIGNAL_Error);
        sError->addTransition(sError, &QState::entered, sIdle);

        // Idle
        sIdle->addTransition(this, &Niveau::SIGNAL_CmdIncrement, sInc);
        sIdle->addTransition(this, &Niveau::SIGNAL_CmdDecrement, sDec);

        // Halt
        QObject::connect(sHalt, &QState::entered, this, [&]() {
            omron_.WriteChannel(CmdStringDecrement(), false);
            omron_.WriteChannel(CmdStringIncrement(), false);
            omron_.WriteChannel(CmdStringWriteOn(), false);
            omron_.WriteChannel(CmdStringWriteOff(), false);
        });
        sHalt->addTransition(sHalt, &QState::entered, sIdle);

        // Inc
        CreateIncState(sInc);
        QObject::connect(sInc, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
        sInc->addTransition(this, &Niveau::SIGNAL_CmdIncrement, sInc);
        sInc->addTransition(sInc, &QState::finished, sIdle);

        // Dec
        CreateDecState(sDec);
        QObject::connect(sDec, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
        sDec->addTransition(this, &Niveau::SIGNAL_CmdDecrement, sDec);
        sDec->addTransition(sDec, &QState::finished, sIdle);

        // SetLevel TODO REMOVE
        CreateSetLevelState(sSetLevel, sError);
        QObject::connect(sSetLevel, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
        QObject::connect(sSetLevel, &QState::entered, this, [&]() { DebugMessage("Ajuster le niveau"); });
        QObject::connect(sSetLevel, &QState::entered, this, [&]() { level_desired_ = level_set_; });
        sSetLevel->addTransition(sSetLevel, &QState::finished, sIdle);

        // CommandOn
        QObject::connect(sCommandOn, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
        QObject::connect(sCommandOn, &QState::entered, this, [&]() { DebugMessage("Procedure on niveau commencé"); });
        sCommandOn->setInitialState(sVerifyOnAllowed);

            // VerifyOnAllowed
            QObject::connect(sVerifyOnAllowed, &QState::entered, this, [&]() { DebugMessage("Vérifier si on niveau autorisé"); });
            QObject::connect(sVerifyOnAllowed, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
            QObject::connect(sVerifyOnAllowed, &QState::entered, this, &Niveau::IsAllowedOn);
            sVerifyOnAllowed->addTransition(this, &Niveau::SIGNAL_OnAllowed_True, sSetPreOnLevel);
            auto tOnNotAllowed = sVerifyOnAllowed->addTransition(this, &Niveau::SIGNAL_OnAllowed_False, sError);
            QObject::connect(tOnNotAllowed, &QSignalTransition::triggered, this, [&]() {
                ErrorMessage("Niveau ON non autorisé");
            });

            // SetPreOnLevel
            CreateSetLevelState(sSetPreOnLevel, sError);
            QObject::connect(sSetPreOnLevel, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
            QObject::connect(sSetPreOnLevel, &QState::entered, this, [&]() { DebugMessage("Ajuster le niveau au pré-allumage"); });
            QObject::connect(sSetPreOnLevel, &QState::entered, this, [&]() { level_desired_ = level_before_on_; });
            sSetPreOnLevel->addTransition(sSetPreOnLevel, &QState::finished, sWriteOn);

            // WriteOn
            sWriteOn->setInitialState(sWriteOn_FlankUp);
            sWriteOn->addTransition(new QSignalTransition(this, &Niveau::SIGNAL_On)); // Swallow
            QObject::connect(sWriteOn, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);

                // WriteOn_FlankUp
                QObject::connect(sWriteOn_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
                QObject::connect(sWriteOn_FlankUp, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringWriteOn(), true);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteOn()); });
                });
                sWriteOn_FlankUp->addTransition(this, &Niveau::SIGNAL_WriteOn_True, sWriteOn_FlankDown);

                // WriteOn_FlankDown
                QObject::connect(sWriteOn_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
                QObject::connect(sWriteOn_FlankDown, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CmdStringWriteOn(), false);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteOn()); });
                });
                auto tWritteOn_Ok = sWriteOn_FlankDown->addTransition(this, &Niveau::SIGNAL_WriteOn_False, sVerifyOn);
                QObject::connect(tWritteOn_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion niveau on écrite"); });

            // VerifyOn
            QObject::connect(sVerifyOn, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
            QObject::connect(sVerifyOn, &QState::entered, this, &Niveau::IsOn);
            // No more actions. If signal on, transition is done. If not, timeout error.

        PrintStateChanges(sError, "Error");
        PrintStateChanges(sIdle, "Idle");
        PrintStateChanges(sHalt, "Halt");
        PrintStateChanges(sInc, "Inc");
        PrintStateChanges(sDec, "Dec");
        PrintStateChanges(sCommandOn, "CommandOn");
        PrintStateChanges(sVerifyOnAllowed, "VerifyOnAllowed");
        PrintStateChanges(sSetPreOnLevel, "SetPreOnLevel");
        PrintStateChanges(sWriteOn, "WriteOn");
        PrintStateChanges(sWriteOn_FlankDown, "WriteOn_FlankDown");
        PrintStateChanges(sWriteOn_FlankUp, "WriteOn_FlankUp");
        PrintStateChanges(sVerifyOn, "VerifyOn");
}

void Niveau::CreateOnState(QState *parent) {
    QState *sError = new QState(parent);
    QState *sIdle = new QState(parent);
    QState *sHalt = new QState(parent);
    global::TimedState *sInc = new global::TimedState(parent, 3000, "Timeout incrément", sError);
    global::TimedState *sDec = new global::TimedState(parent, 3000, "Timeout décrement", sError);
    global::TimedState *sSetStartupFinalLevel = new global::TimedState(parent, 30000, "Echec du réglage du niveau", sError);
    global::TimedState *sSetShutdownInitialLevel = new global::TimedState(parent, 30000, "Echec du réglage du niveau", sError);
    global::TimedState *sCommandOff = new global::TimedState(parent, 30000, "Délai d'attente pour mettre niveau off", sError);
       global::TimedState *sSetPreOffLevel = new global::TimedState(sCommandOff, 12000, "Echec du réglage du niveau", sError);
       global::TimedState *sWriteOff = new global::TimedState(sCommandOff, 6000, "Nombre maximum d'essais de niveau off atteint", sError);
           global::TimedState *sWriteOff_FlankUp = new global::TimedState(sWriteOff, 1000, "Réessayer la commande off niveau flanc montant");
           global::TimedState *sWriteOff_FlankDown = new global::TimedState(sWriteOff, 1000, "Réessayer la commande off niveau flanc descendent");
       global::TimedState *sVerifyOff = new global::TimedState(sCommandOff, 5000, "Echec de la vérification si le niveau off", sError);

       // parent
       parent->setInitialState(sIdle);
       parent->addTransition(this, &Niveau::SIGNAL_CmdOff, sCommandOff)->setTransitionType(QAbstractTransition::InternalTransition);
       parent->addTransition(this, &Niveau::SIGNAL_SetStartupFinalValue, sSetStartupFinalLevel)->setTransitionType(QAbstractTransition::InternalTransition);
       parent->addTransition(this, &Niveau::SIGNAL_SetShutdownInitialValue, sSetShutdownInitialLevel)->setTransitionType(QAbstractTransition::InternalTransition);
       parent->addTransition(this, &Niveau::SIGNAL_Interrupt, sHalt)->setTransitionType(QAbstractTransition::InternalTransition);

       // Error
       QState::connect(sError, &QState::entered, this, &Niveau::SIGNAL_Error);
       sError->addTransition(sError, &QState::entered, sIdle);

       // Idle
       sIdle->addTransition(this, &Niveau::SIGNAL_CmdIncrement, sInc);
       sIdle->addTransition(this, &Niveau::SIGNAL_CmdDecrement, sDec);

       // Halt
       QObject::connect(sHalt, &QState::entered, this, [&]() {
           omron_.WriteChannel(CmdStringDecrement(), false);
           omron_.WriteChannel(CmdStringIncrement(), false);
       });
       sHalt->addTransition(sHalt, &QState::entered, sIdle);

       // Inc
       CreateIncState(sInc);
       QObject::connect(sInc, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
       sInc->addTransition(this, &Niveau::SIGNAL_CmdIncrement, sInc);
       sInc->addTransition(sInc, &QState::finished, sIdle);

       // Dec
       CreateDecState(sDec);
       QObject::connect(sDec, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
       sDec->addTransition(this, &Niveau::SIGNAL_CmdDecrement, sDec);
       sDec->addTransition(sDec, &QState::finished, sIdle);

       // SetStartupFinalLevel
       CreateSetLevelState(sSetStartupFinalLevel, sError);
       QObject::connect(sSetStartupFinalLevel, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
       QObject::connect(sSetStartupFinalLevel, &QState::entered, this, [&]() { DebugMessage("Mettre le consigne startup final"); });
       QObject::connect(sSetStartupFinalLevel, &QState::entered, this, [&]() { level_desired_ = level_startup_final_; });
       sSetStartupFinalLevel->addTransition(sSetStartupFinalLevel, &QState::finished, sIdle);

       // SetShutdownInitialLevel
       CreateSetLevelState(sSetShutdownInitialLevel, sError);
       QObject::connect(sSetShutdownInitialLevel, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
       QObject::connect(sSetShutdownInitialLevel, &QState::entered, this, [&]() { DebugMessage("Mettre le consigne shutdown initial"); });
       QObject::connect(sSetShutdownInitialLevel, &QState::entered, this, [&]() { level_desired_ = level_shutdown_initial_; });
       sSetShutdownInitialLevel->addTransition(sSetShutdownInitialLevel, &QState::finished, sIdle);

       // CommandOff
       QObject::connect(sCommandOff, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
       QObject::connect(sCommandOff, &QState::entered, this, [&]() { DebugMessage("Procedure off niveau commencé"); });
       sCommandOff->setInitialState(sSetPreOffLevel);

           // SetPreOffLevel
           CreateSetLevelState(sSetPreOffLevel, sError);
           QObject::connect(sSetPreOffLevel, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
           QObject::connect(sSetPreOffLevel, &QState::entered, this, [&]() { DebugMessage("Ajuster le niveau au pré-desallumage"); });
           QObject::connect(sSetPreOffLevel, &QState::entered, this, [&]() { level_desired_ = level_before_off_; });
           sSetPreOffLevel->addTransition(sSetPreOffLevel, &QState::finished, sWriteOff);

           // WriteOff
           sWriteOff->setInitialState(sWriteOff_FlankUp);
           sWriteOff->addTransition(new QSignalTransition(this, &Niveau::SIGNAL_Off)); // Swallow
           QObject::connect(sWriteOff, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);

               // WriteOff_FlankUp
               QObject::connect(sWriteOff_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
               QObject::connect(sWriteOff_FlankUp, &QState::entered, this, [&]() {
                   omron_.WriteChannel(CmdStringWriteOff(), true);
                   QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteOff()); });
               });
               sWriteOff_FlankUp->addTransition(this, &Niveau::SIGNAL_WriteOff_True, sWriteOff_FlankDown);

               // WriteOff_FlankDown
               QObject::connect(sWriteOff_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
               QObject::connect(sWriteOff_FlankDown, &QState::entered, this, [&]() {
                   omron_.WriteChannel(CmdStringWriteOff(), false);
                   QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteOff()); });
               });
               auto tWriteOff_Ok = sWriteOff_FlankDown->addTransition(this, &Niveau::SIGNAL_WriteOff_False, sVerifyOff);
               QObject::connect(tWriteOff_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion niveau off écrite"); });

           // VerifyOff
           QObject::connect(sVerifyOff, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
           QObject::connect(sVerifyOff, &QState::entered, this, &Niveau::IsOn);
           // No more actions. If signal off, transition is done. If not, timeout error.

           PrintStateChanges(sError, "Error");
           PrintStateChanges(sIdle, "Idle");
           PrintStateChanges(sHalt, "Halt");
           PrintStateChanges(sInc, "Inc");
           PrintStateChanges(sDec, "Dec");
           PrintStateChanges(sSetStartupFinalLevel, "SetStartupFinalLevel");
           PrintStateChanges(sCommandOff, "CommandOff");
           PrintStateChanges(sSetPreOffLevel, "SetPreOffLevel");
           PrintStateChanges(sWriteOff, "WriteOff");
           PrintStateChanges(sWriteOff_FlankDown, "WriteOff_FlankDown");
           PrintStateChanges(sWriteOff_FlankUp, "WriteOff_FlankUp");
           PrintStateChanges(sVerifyOff, "VerifyOff");
}

void Niveau::CreateSetLevelState(QState *parent, QState *error) {
    global::TimedState *sCheckDirection = new global::TimedState(parent, 3000, "Timeout lecture niveau");
    global::TimedState *sDecUntilBelow = new global::TimedState(parent, 45000, "Timeout décrement niveau", error);
        global::TimedState *sWriteDecFlankUp = new global::TimedState(sDecUntilBelow, 2000, "Echec d'écriture niveau front montant");
        global::TimedState *sWaitUntilBelow = new global::TimedState(sDecUntilBelow, 300, "Read next value");
        global::TimedState *sWriteDecFlankDown = new global::TimedState(sDecUntilBelow, 2000, "Echec d'écriture du front déscendent");
    global::TimedState *sIncUntilAbove = new global::TimedState(parent, 45000, "Timeout incrément niveau", error);
        global::TimedState *sWriteIncFlankUp = new global::TimedState(sIncUntilAbove, 2000, "Echec d'écriture niveau front montant");
        global::TimedState *sWaitUntilAbove = new global::TimedState(sIncUntilAbove, 300, "Read next value");
        global::TimedState *sWriteIncFlankDown = new global::TimedState(sIncUntilAbove, 2000, "Echec d'écriture niveau front déscendent");
    QFinalState *sFinished = new QFinalState(parent);

    // Parent
    parent->setInitialState(sCheckDirection);

    // CheckDirection
    QObject::connect(sCheckDirection, &QState::entered, this, &Niveau::ReadValue);
    sCheckDirection->addTransition(this, &Niveau::SIGNAL_ActAboveDes, sDecUntilBelow);
    sCheckDirection->addTransition(this, &Niveau::SIGNAL_ActBelowDes, sIncUntilAbove);

    // DecUntilBelow
    QObject::connect(sDecUntilBelow, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
    sDecUntilBelow->setInitialState(sWriteDecFlankUp);

        // WriteDecFlankUp
        QObject::connect(sWriteDecFlankUp, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
        QObject::connect(sWriteDecFlankUp, &QState::entered, this, [&]() {
            omron_.WriteChannel(CmdStringDecrement(), true);
            QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringDecrement()); });
        });
        sWriteDecFlankUp->addTransition(this, &Niveau::SIGNAL_Decrement_True, sWaitUntilBelow);

        // WaitUntilBelow
        QObject::connect(sWaitUntilBelow, &QState::entered, this, &Niveau::ReadValue);
        auto tActBelowDes = sWaitUntilBelow->addTransition(this, &Niveau::SIGNAL_ActBelowDes, sWriteDecFlankDown);
        QObject::connect(tActBelowDes, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau réel proche du niveau souhaité"); });

        // WriteDecFlankDown
        QObject::connect(sWriteDecFlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
        QObject::connect(sWriteDecFlankDown, &QState::entered, this, [&]() {
            omron_.WriteChannel(CmdStringDecrement(), false);
            QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringDecrement()); });
        });
        sWriteDecFlankDown->addTransition(this, &Niveau::SIGNAL_Decrement_False, sFinished);

    // IncUntilAbove
    QObject::connect(sIncUntilAbove, &global::TimedState::SIGNAL_Timeout, this, &Niveau::ErrorMessage);
    sIncUntilAbove->setInitialState(sWriteIncFlankUp);

        // WriteIncFlankUp
        QObject::connect(sWriteIncFlankUp, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
        QObject::connect(sWriteIncFlankUp, &QState::entered, this, [&]() {
            omron_.WriteChannel(CmdStringIncrement(), true);
            QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringIncrement()); });
        });
        sWriteIncFlankUp->addTransition(this, &Niveau::SIGNAL_Increment_True, sWaitUntilAbove);

        // WaitUntilAbove
        QObject::connect(sWaitUntilAbove, &QState::entered, this, &Niveau::ReadValue);
        auto tActAboveDes = sWaitUntilAbove->addTransition(this, &Niveau::SIGNAL_ActAboveDes, sWriteIncFlankDown);
        QObject::connect(tActAboveDes, &QSignalTransition::triggered, this, [&]() { DebugMessage("Niveau réel proche du niveau souhaité"); });
        auto tActAboveMax = sWaitUntilAbove->addTransition(this, &Niveau::SIGNAL_ActAboveMax, sWriteIncFlankDown);
        QObject::connect(tActAboveMax, &QSignalTransition::triggered, this, [&]() { WarningMessage("Niveau max atteint"); });

        // WriteIncFlankDown
        QObject::connect(sWriteIncFlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
        QObject::connect(sWriteIncFlankDown, &QState::entered, this, [&]() {
            omron_.WriteChannel(CmdStringIncrement(), false);
            QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringIncrement()); });
        });
        sWriteIncFlankDown->addTransition(this, &Niveau::SIGNAL_Increment_False, sFinished);

    // Finished
    QObject::connect(sFinished, &QState::entered, this, [&]() { DebugMessage("Niveau souhaité atteint"); });
    QObject::connect(sFinished, &QState::entered, this, [&]() { emit SIGNAL_ValueReached(); });

    PrintStateChanges(sCheckDirection, "CheckDirection");
    PrintStateChanges(sDecUntilBelow, "DecUntilBelow");
    PrintStateChanges(sWriteDecFlankUp, "WriteDecFlankUp");
    PrintStateChanges(sWaitUntilBelow, "WaitUntilBelow");
    PrintStateChanges(sWriteDecFlankDown, "WriteDecFlankDown");
    PrintStateChanges(sIncUntilAbove, "IncUntilAbove");
    PrintStateChanges(sWriteIncFlankUp, "WriteIncFlankUp");
    PrintStateChanges(sWaitUntilAbove, "WaitUntilAbove");
    PrintStateChanges(sWriteIncFlankDown, "WriteIncFlankDown");
    PrintStateChanges(sFinished, "Finished");
}

void Niveau::CreateIncState(QState *parent) {
    QState *sWriteIncFlankUp = new QState(parent);
    global::TimedState *sWriteIncFlankDown = new global::TimedState(parent, 1000, "Echec d'écriture du front déscendent");
    global::TimedState *sPulseHigh = new global::TimedState(parent, incdec_pulse_length_, "Goto next", sWriteIncFlankDown);
    QFinalState *sFinished = new QFinalState(parent);

    // parent
    parent->setInitialState(sWriteIncFlankUp);

    // WriteIncFlankUp
    QObject::connect(sWriteIncFlankUp, &QState::entered, this, [&]() {
        DebugMessage("Incrément");
        omron_.WriteChannel(CmdStringIncrement(), true);
    });
    sWriteIncFlankUp->addTransition(sWriteIncFlankUp, &QState::entered, sPulseHigh);

    // PulseHigh
    // No more actions

    // WriteIncFlankDown
    QObject::connect(sWriteIncFlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
    QObject::connect(sWriteIncFlankDown, &QState::entered, this, [&]() {
        omron_.WriteChannel(CmdStringIncrement(), false);
        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringIncrement()); });
    });
    sWriteIncFlankDown->addTransition(this, &Niveau::SIGNAL_Increment_False, sFinished);

    PrintStateChanges(sWriteIncFlankUp, "IncState::WriteIncFlankUp");
    PrintStateChanges(sWriteIncFlankDown, "IncState::WriteIncFlankDown");
    PrintStateChanges(sPulseHigh, "IncState::PulseHigh");
    PrintStateChanges(sFinished, "IncState::Finished");
}

void Niveau::CreateDecState(QState *parent) {
    QState *sWriteDecFlankUp = new QState(parent);
    global::TimedState *sWriteDecFlankDown = new global::TimedState(parent, 1000, "Echec d'écriture du front déscendent");
    global::TimedState *sPulseHigh = new global::TimedState(parent, incdec_pulse_length_, "Goto next", sWriteDecFlankDown);
    QFinalState *sFinished = new QFinalState(parent);

    // parent
    parent->setInitialState(sWriteDecFlankUp);

    // WriteDecFlankUp
    QObject::connect(sWriteDecFlankUp, &QState::entered, this, [&]() {
        DebugMessage("Décrement");
        omron_.WriteChannel(CmdStringDecrement(), true);
    });
    sWriteDecFlankUp->addTransition(sWriteDecFlankUp, &QState::entered, sPulseHigh);

    // PulseHigh
    // No more actions

    // WriteDecFlankDown
    QObject::connect(sWriteDecFlankDown, &global::TimedState::SIGNAL_Timeout, this, &Niveau::WarningMessage);
    QObject::connect(sWriteDecFlankDown, &QState::entered, this, [&]() {
        omron_.WriteChannel(CmdStringDecrement(), false);
        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringDecrement()); });
    });
    sWriteDecFlankDown->addTransition(this, &Niveau::SIGNAL_Decrement_False, sFinished);

    PrintStateChanges(sWriteDecFlankUp, "DecState::WriteDecFlankUp");
    PrintStateChanges(sWriteDecFlankDown, "DecState::WriteDecFlankDown");
    PrintStateChanges(sPulseHigh, "DecState::PulseHigh");
    PrintStateChanges(sFinished, "DecState::Finished");
}


void Niveau::ConnectSignals() {
    qDebug() << "Niveau::ConnectSignals";
    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead , this, &Niveau::InterpretOmronBitRead);
    QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, &Niveau::InterpretADCChannelRead);
    QObject::connect(this, &Niveau::SIGNAL_Value, this, &Niveau::CheckValueThresholds);
}

void Niveau::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Niveau " + GetHFBranch() + " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Niveau " + GetHFBranch() +" <-" << name; });
}

void Niveau::CheckValueThresholds(double value) {
    // Max level
    if (value < level_max_) {
        emit SIGNAL_ActBelowMax();
    } else {
        emit SIGNAL_ActAboveMax();
    }

    // Desired level
    if (value <= level_desired_) {
        emit SIGNAL_ActBelowDes();
    } else {
        emit SIGNAL_ActAboveDes();
    }

    // Shutdown initial level
    if (value <= level_shutdown_initial_) {
        emit SIGNAL_BelowShutdownInitialValue();
    } else {
        emit SIGNAL_AboveShutdownInitialValue();
    }
}

void Niveau::InterpretADCChannelRead(const QString& channel, double value) {
    if (channel == CmdStringReadValue()) {
        level_act_ = value;
        emit SIGNAL_Value(value);
    } // else no action
}

void Niveau::InterpretOmronBitRead(const QString& channel, bool content) {
    if (channel == CmdStringIncrement()) {
        content ? emit SIGNAL_Increment_True() : emit SIGNAL_Increment_False();
    } else if (channel == CmdStringDecrement()) {
        content ? emit SIGNAL_Decrement_True() : emit SIGNAL_Decrement_False();
    } else if (channel == CmdStringIsAllowedOn()) {
        content ? emit SIGNAL_OnAllowed_False() : emit SIGNAL_OnAllowed_True();
    } else if(channel == CmdStringIsOn()) {
        content ? emit SIGNAL_On() : emit SIGNAL_Off();
    } else if (channel == CmdStringWriteOn()) {
        content ? emit SIGNAL_WriteOn_True() : emit SIGNAL_WriteOn_False();
    } else if (channel == CmdStringWriteOff()) {
        content ? emit SIGNAL_WriteOff_True() : emit SIGNAL_WriteOff_False();
    } // else no action
}

void Niveau::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Niveau::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Niveau::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Niveau::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

} // namespace
