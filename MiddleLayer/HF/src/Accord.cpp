#include "Accord.h"

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

Accord::Accord(HFX hfx, Membrane* membrane, Dee* dee, Niveau* niveau)
    : hfx_(hfx), membrane_(membrane), dee_(dee), niveau_(niveau),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    ConnectSignals();
    SetupStateMachine();
}

void Accord::Configure_VoltageLower(double value) {
    qDebug() << "Accord::Configure_VoltageLower " << value;
    voltage_lower_ = value;
}

void Accord::Configure_VoltageUpper(double value) {
    qDebug() << "Accord::Configure_VoltageUpper " << value;
    voltage_upper_ = value;
}

void Accord::Configure_PhaseLower(double value) {
    qDebug() << "Accord::Configure_PhaseLower " << value;
    phase_lower_ = value;
}

void Accord::Configure_PhaseUpper(double value) {
    qDebug() << "Accord::Configure_PhaseUpper " << value;
    phase_upper_ = value;
}

void Accord::Ping() {
    dee_->Ping();
}

void Accord::ConnectSignals() {
    QObject::connect(dee_, &Dee::SIGNAL_DeeVoltage, this, [&](double voltage) {
        voltage_ = voltage;
        CheckVoltageLimits();
        CheckAccord();
    });
    QObject::connect(dee_, &Dee::SIGNAL_DeePhase, this, [&](double phase) {
        phase_ = phase;
        CheckAccord();
    });
}

void Accord::CheckVoltageLimits() {
    if (voltage_ > voltage_lower_) {
        emit SIGNAL_VoltageAboveLowerLimit();
    } else {
        emit SIGNAL_VoltageBelowLowerLimit();
    }
    if (voltage_ > voltage_upper_) {
        emit SIGNAL_VoltageAboveUpperLimit();
    } else {
        emit SIGNAL_VoltageBelowUpperLimit();
    }
}

void Accord::CheckAccord() {
    if (phase_ >= phase_lower_ && phase_ <= phase_upper_ && voltage_ > voltage_lower_) {
        emit SIGNAL_Accorded();
    } else {
        emit SIGNAL_NotAccorded();
    }
}

void Accord::FindAccord() {
    emit SIGNAL_FindAccord();
}

void Accord::Interrupt() {
    emit SIGNAL_Interrupt();
}

void Accord::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sError = new QState(sSuperState);
        global::TimedState *sNotAccorded = new global::TimedState(sSuperState, 1000, "Reentry");
        global::TimedState *sAccorded = new global::TimedState(sSuperState, 1000, "Reentry");
        global::TimedState *sFindAccord = new global::TimedState(sSuperState, 4*60000, "Accord pas trouvé (signal et/ou phase)", sError);
            global::TimedState *sSetMembraneManual = new global::TimedState(sFindAccord, 10000, "La recherche de l'accord s'est arrêtée (SetMembraneManual)", sError);
            global::TimedState *sMoveUp = GetScanState(sFindAccord, 180000, "La recherche de l'accord s'est arrêtée (MoveUp)", sError);
            global::TimedState *sMoveDown = GetScanState(sFindAccord, 180000, "La recherche de l'accord s'est arrêtée (MoveDown)", sError);
            global::TimedState *sStop = new global::TimedState(sFindAccord, 10000, "La recherche de l'accord s'est arrêtée (Stop)", sError);
            global::TimedState *sSetMembraneAuto = new global::TimedState(sFindAccord, 10000, "La recherche de l'accord s'est arrêtée (SetMembraneAuto)", sError);
            global::TimedState *sCheckAccord = new global::TimedState(sFindAccord, 10000, "La recherche de l'accord s'est arrêtée (CheckAccord)", sError);

     // SuperState
     sSuperState->setInitialState(sNotAccorded);
     sSuperState->addTransition(this, &Accord::SIGNAL_FindAccord, sFindAccord);

         // Error
         QObject::connect(sError, &QState::entered, this, &Accord::SIGNAL_Error);
         sError->addTransition(sError, &QState::entered, sNotAccorded);

         // NotAccorded
         QObject::connect(sNotAccorded, &global::TimedState::SIGNAL_Timeout, this, &Accord::SIGNAL_NotAccorded);
         auto tFoundAccord = sNotAccorded->addTransition(this, &Accord::SIGNAL_Accorded, sAccorded);
         QObject::connect(tFoundAccord, &QSignalTransition::triggered, this, [&]() { DebugMessage("Accord (re)trouvé"); });

         // Accorded
         QObject::connect(sAccorded, &global::TimedState::SIGNAL_Timeout, this, &Accord::SIGNAL_Accorded);
         auto tLostAccord = sAccorded->addTransition(this, &Accord::SIGNAL_NotAccorded, sNotAccorded);
         QObject::connect(tLostAccord, &QSignalTransition::triggered, this, [&]() { DebugMessage("Accord perdu"); });

         // FindAccord
         QObject::connect(sFindAccord, &QState::entered, this, [&]() { DebugMessage("Rechercher l'accord"); });
         sFindAccord->setInitialState(sSetMembraneManual);
         auto tMembraneErrorInFindAccord = sFindAccord->addTransition(membrane_, &Membrane::SIGNAL_Error, sError);
         QObject::connect(tMembraneErrorInFindAccord, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Recherche l'accord interrompue par une erreur"); });
         auto tDeeErrorInFindAccord = sFindAccord->addTransition(dee_, &Dee::SIGNAL_Error, sError);
         QObject::connect(tDeeErrorInFindAccord, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Recherche l'accord interrompue par une errue"); });
         auto tNiveauErrorInFindAccord = sFindAccord->addTransition(niveau_, &Niveau::SIGNAL_Error, sError);
         QObject::connect(tNiveauErrorInFindAccord, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Recherche l'accord interrompue par une errue"); });
         auto tInterrupted = sFindAccord->addTransition(this, &Accord::SIGNAL_Interrupt, sNotAccorded);
         QObject::connect(tInterrupted, &QSignalTransition::triggered, this, [&]() { WarningMessage("Recherche l'accord interrompue"); });

            // SetMembraneManual
            QObject::connect(sSetMembraneManual, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            QObject::connect(sSetMembraneManual, &QState::entered, membrane_, &Membrane::SetModeManual);
            sSetMembraneManual->addTransition(membrane_, &Membrane::SIGNAL_Manual, sMoveUp);

            // MoveUp
            QObject::connect(sMoveUp, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            QObject::connect(sMoveUp, &QState::entered, membrane_, &Membrane::StartMovingUp);
            sMoveUp->addTransition(membrane_, &Membrane::SIGNAL_SWUpperLimitReached, sMoveDown);
            sMoveUp->addTransition(this, &Accord::SIGNAL_Accorded, sStop);
            auto tMoveDown = sMoveUp->addTransition(dee_, &Dee::SIGNAL_VoltageDecreasing, sMoveDown);
            QObject::connect(tMoveDown, &QSignalTransition::triggered, this, [&]() { InfoMessage("La tension HF diminue. Rechercher l'accord dans l'autre sens"); });

            // MoveDown
            QObject::connect(sMoveDown, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            QObject::connect(sMoveDown, &QState::entered, membrane_, &Membrane::StartMovingDown);
            sMoveDown->addTransition(membrane_, &Membrane::SIGNAL_SWLowerLimitReached, sMoveUp);
            sMoveDown->addTransition(this, &Accord::SIGNAL_Accorded, sStop);
            auto tMoveUp = sMoveDown->addTransition(dee_, &Dee::SIGNAL_VoltageDecreasing, sMoveUp);
            QObject::connect(tMoveUp, &QSignalTransition::triggered, this, [&]() { InfoMessage("La tension HF diminue. Rechercher l'accord dans l'autre sens"); });

            // Stop
            QObject::connect(sStop, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            QObject::connect(sStop, &QState::entered, membrane_, &Membrane::Stop);
            sStop->addTransition(membrane_, &Membrane::SIGNAL_Stopped, sSetMembraneAuto);

            // SetMembraneAuto
            QObject::connect(sSetMembraneAuto, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            QObject::connect(sSetMembraneAuto, &QState::entered, this, [&]() { DebugMessage("Phase et tension ok, activer le mode auto"); });
            QObject::connect(sSetMembraneAuto, &QState::entered, membrane_, &Membrane::SetModeAutomatic);
            sSetMembraneAuto->addTransition(membrane_, &Membrane::SIGNAL_Automatic, sCheckAccord);

            // CheckAccord
            QObject::connect(sCheckAccord, &global::TimedState::SIGNAL_Timeout, this, &Accord::ErrorMessage);
            auto tAccordConfirmed = sCheckAccord->addTransition(this, &Accord::SIGNAL_Accorded, sAccorded);
            QObject::connect(tAccordConfirmed, &QSignalTransition::triggered, this, [&]() {
                membrane_->SaveAccordPosition();
                DebugMessage("Accord trouvé");
            });
            auto tMissedAccord = sCheckAccord->addTransition(this, &Accord::SIGNAL_NotAccorded, sMoveUp);
            QObject::connect(tMissedAccord, &QSignalTransition::triggered, this, [&]() {
                WarningMessage("Accord perdu, essayer à nouveau");
            });

            PrintStateChanges(sSuperState, "SuperState");
            PrintStateChanges(sError, "Error");
//            PrintStateChanges(sNotAccorded, "NotAccorded");
//            PrintStateChanges(sAccorded, "Accorded");
            PrintStateChanges(sFindAccord, "FindAccord");
            PrintStateChanges(sSetMembraneManual, "SetMembraneManual");
            PrintStateChanges(sMoveUp, "MoveUp");
            PrintStateChanges(sMoveDown, "MoveDown");
            PrintStateChanges(sStop, "Stop");
            PrintStateChanges(sSetMembraneAuto, "SetMembraneAuto");
            PrintStateChanges(sCheckAccord, "CheckAccord");

            sm_.addState(sSuperState);
            sm_.setInitialState(sSuperState);
            sm_.start();
}

global::TimedState* Accord::GetScanState(QState *parent, int timeout_ms, QString timeout_message, QState* timeout_state) {
    global::TimedState *sScan = new global::TimedState(parent, timeout_ms, timeout_message, timeout_state);
        QState *sCruise = new QState(sScan);
        QState *sDecrementNiveau = new QState(sScan);

    // Scan
    sScan->setInitialState(sCruise);

        // Cruise
        sCruise->addTransition(this, &Accord::SIGNAL_VoltageAboveUpperLimit, sDecrementNiveau);

        // DecrementNiveau
        QObject::connect(sDecrementNiveau, &QState::entered, niveau_, &Niveau::Decrement);
        sDecrementNiveau->addTransition(sDecrementNiveau, &QState::entered, sCruise);

    PrintStateChanges(sScan, "Scan");
    PrintStateChanges(sCruise, "Cruise");
    PrintStateChanges(sDecrementNiveau, "DecrementNiveau");

    return sScan;
}

void Accord::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Accord::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Accord::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Accord::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Accord::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Accord ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Accord <-" << name; });
}

QString Accord::GetHFBranch() {
     return QString("HF") + QString::number(static_cast<int>(hfx_));
}

} // ns


