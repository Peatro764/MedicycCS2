#include "VoltageRegulator.h"

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

VoltageRegulator::VoltageRegulator(HFX hfx, Dee* dee, Niveau* niveau)
    : hfx_(hfx), dee_(dee), niveau_(niveau),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    ConnectSignals();
    SetupStateMachine();
}

void VoltageRegulator::Configure_VoltageMax(double value) {
    voltage_max_ = value;
    qDebug() << "VoltageRegulator::Configure_VoltageMax " << value;
}

void VoltageRegulator::ConnectSignals() {
    QObject::connect(dee_, &Dee::SIGNAL_DeeVoltage, this, &VoltageRegulator::UpdateDeeVoltage);
}

void VoltageRegulator::UpdateDeeVoltage(double kv) {
    voltage_actual_ = kv;
    if (voltage_actual_ >= voltage_max_) {
        emit SIGNAL_ActAboveMax();
    }
    if (voltage_actual_ >= voltage_setpoint_) {
        emit SIGNAL_ActAboveSet();
    } else {
        emit SIGNAL_ActBelowSet();
    }
}

void VoltageRegulator::SetVoltage(double kV) {
    qDebug() << "VoltageRegulator::SetVoltage act " << voltage_actual_ << " max " << voltage_max_;
    voltage_setpoint_ = kV;
    emit SIGNAL_SetPointUpdated();
}

void VoltageRegulator::IncrementVoltage(double kV) {
    if (voltage_actual_ < voltage_max_) {
        double des_voltage = std::min(voltage_actual_ + kV, voltage_max_);
        SetVoltage(std::max(des_voltage, MIN_VOLTAGE_));
    } else {
        qWarning() << "VoltageRegulator::IncrementVoltage Voltage above max, no action";
        emit SIGNAL_SetPointAboveMax();
    }
}

void VoltageRegulator::DecrementVoltage(double kV) {
    qDebug() << "VoltageRegulator::DecrementVoltage " << kV;
    // Cannot decrement to 0 voltage (voltage at niveau = 0 is still positive)
    SetVoltage(std::max(voltage_actual_ - kV, MIN_VOLTAGE_));
}

void VoltageRegulator::Interrupt() {
    emit SIGNAL_Interrupt();
}

void VoltageRegulator::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        QState *sError = new QState(sSuperState);
        global::TimedState *sRegulate = new global::TimedState(sSuperState, 60000, "Délai d'attente pour atteindre la tension souhaitée", sError);
            global::TimedState *sDetermineDirection = new global::TimedState(sRegulate, 10000, "Délai d'attente pour la lecture de la tension (DetermineDirection)", sError);
            global::TimedState *sCheckIfActAboveSet = new global::TimedState(sRegulate, 10000, "Délai d'attente pour la lecture de la tension (CheckIfActAbove)", sError);
            global::TimedState *sIncrement = new global::TimedState(sRegulate, 1200, "Wait for voltage to increase", sCheckIfActAboveSet);
            global::TimedState *sCheckIfActBelowSet = new global::TimedState(sRegulate, 10000, "Délai d'attente pour la lecture de la tension (CheckIfActBelowSet)", sError);
            global::TimedState *sDecrement = new global::TimedState(sRegulate, 1200, "Wait for voltage to increase", sCheckIfActBelowSet);
        QState *sDone = new QState(sSuperState);

        // SuperState
        sSuperState->setInitialState(sIdle);
        sSuperState->addTransition(this, &VoltageRegulator::SIGNAL_SetPointUpdated, sRegulate);
        sSuperState->addTransition(this, &VoltageRegulator::SIGNAL_Interrupt, sIdle);

        // Idle
        // no actions

        // Error
        QObject::connect(sError, &QState::entered, this, &VoltageRegulator::SIGNAL_Error);
        sError->addTransition(sError, &QState::entered, sIdle);

        // Regulate
        QObject::connect(sRegulate, &global::TimedState::SIGNAL_Timeout, this, &VoltageRegulator::ErrorMessage);
        sRegulate->setInitialState(sDetermineDirection);
        sRegulate->addTransition(dee_, &Dee::SIGNAL_Error, sError);
        sRegulate->addTransition(niveau_, &Niveau::SIGNAL_Error, sError);

            // DetermineDirection
            QObject::connect(sDetermineDirection, &global::TimedState::SIGNAL_Timeout, this, &VoltageRegulator::ErrorMessage);
            QObject::connect(sDetermineDirection, &QState::entered, dee_, &Dee::Ping);
            sDetermineDirection->addTransition(this, &VoltageRegulator::SIGNAL_ActAboveSet, sDecrement);
            sDetermineDirection->addTransition(this, &VoltageRegulator::SIGNAL_ActBelowSet, sCheckIfActAboveSet);

            // Increment
            // State transitions to checkifactaboveset after a given timeout
            QObject::connect(sIncrement, &QState::entered, niveau_, &Niveau::Increment);

            // CheckIfActAboveSet
            QObject::connect(sCheckIfActAboveSet, &global::TimedState::SIGNAL_Timeout, this, &VoltageRegulator::ErrorMessage);
            QObject::connect(sCheckIfActAboveSet, &QState::entered, dee_, &Dee::Ping);
            sCheckIfActAboveSet->addTransition(this, &VoltageRegulator::SIGNAL_ActAboveMax, sDone);
            sCheckIfActAboveSet->addTransition(this, &VoltageRegulator::SIGNAL_ActAboveSet, sDone);
            sCheckIfActAboveSet->addTransition(this, &VoltageRegulator::SIGNAL_ActBelowSet, sIncrement);

            // Decrement
            // State transitions to checkifactbelowset after a given timeout
            QObject::connect(sDecrement, &QState::entered, niveau_, &Niveau::Decrement);

            // CheckIfActBelowSet
            QObject::connect(sCheckIfActBelowSet, &global::TimedState::SIGNAL_Timeout, this, &VoltageRegulator::ErrorMessage);
            QObject::connect(sCheckIfActBelowSet, &QState::entered, dee_, &Dee::Ping);
            sCheckIfActBelowSet->addTransition(this, &VoltageRegulator::SIGNAL_ActBelowSet, sDone);
            sCheckIfActBelowSet->addTransition(this, &VoltageRegulator::SIGNAL_ActAboveSet, sDecrement);

            // Done
            QObject::connect(sDone, &QState::entered, this, &VoltageRegulator::SIGNAL_VoltageSet);
            QObject::connect(sDone, &QState::entered, this, [&]() { DebugMessage("La tension souhaitée est réglée"); });
            sDone->addTransition(sDone, &QState::entered, sIdle);

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sError, "Error");
    PrintStateChanges(sRegulate, "Regulate");
    PrintStateChanges(sDetermineDirection, "DetermineDirection");
    PrintStateChanges(sCheckIfActAboveSet, "CheckIfActAboveSet");
    PrintStateChanges(sIncrement, "Increment");
    PrintStateChanges(sCheckIfActBelowSet, "CheckIfActBelowSet");
    PrintStateChanges(sDecrement, "Decrement");

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void VoltageRegulator::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void VoltageRegulator::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void VoltageRegulator::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void VoltageRegulator::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void VoltageRegulator::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "VoltageRegulator ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "VoltageRegulator <-" << name; });
}

QString VoltageRegulator::GetHFBranch() {
     return QString("HF") + QString::number(static_cast<int>(hfx_));
}

} // ns


