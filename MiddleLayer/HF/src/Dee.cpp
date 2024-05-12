#include "Dee.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

Dee::Dee(HFX hfx) :
    hfx_(hfx),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.hf", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    ConnectSignals();
    SetupStateMachine();
}

Dee::~Dee() {

}

void Dee::SetRegulationOn() {
    emit SIGNAL_CmdRegulationOn();
}

void Dee::SetRegulationOff() {
    emit SIGNAL_CmdRegulationOff();
}

void Dee::IsRegulationOn() {
    omron_.ReadChannel(CmdStringIsRegulationOn());
}

void Dee::ReadDeeVoltage() {
    adc_.Read(CmdStringReadDeeVoltage());
}

void Dee::ReadDeePhase() {
    adc_.Read(CmdStringReadDeePhase());
}

QString Dee::GetHFBranch() const {
    return QString("HF%1").arg(static_cast<int>(hfx_));
}

void Dee::Ping() {
    qDebug() << QDateTime::currentDateTime() << " Dee::Ping";
    IsRegulationOn();
    ReadDeeVoltage();
    ReadDeePhase();
    (omron_.isValid() && adc_.isValid()) ? emit SIGNAL_Connected() : emit SIGNAL_Disconnected();
}

void Dee::ConnectSignals() {
    QObject::connect(this, &Dee::SIGNAL_DeePhase, this, [&](double value) {
       phase_ = value;
    });
    QObject::connect(this, &Dee::SIGNAL_DeeVoltage, this, [&](double value) {
        CheckVoltage(value);
        voltage_ = value;
    });
    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead , this, &Dee::InterpretOmronBitRead);
    QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, &Dee::InterpretADCChannelRead);
}

void Dee::SetupStateMachine() {

    QState *sSuperState = new QState();
        QState *sDisconnected = new QState(sSuperState);
        QState *sConnected = new QState(sSuperState);
            QState *sIdle = new QState(sConnected);
            QState *sError = new QState(sConnected);
            global::TimedState *sWriteRegulOff = new global::TimedState(sConnected, 6000, "Nombre maximum d'essais de regulation off atteint", sError);
                global::TimedState *sWriteRegulOff_FlankUp = new global::TimedState(sWriteRegulOff, 1000, "Réessayer la commande off regul flanc montant");
                global::TimedState *sWriteRegulOff_FlankDown = new global::TimedState(sWriteRegulOff, 1000, "Réessayer la commande off regul flanc descendent");
                global::TimedState *sVerifyRegulOff = new global::TimedState(sWriteRegulOff, 5000, "Echec de la vérification si le regul off", sError);
            global::TimedState *sWriteRegulOn = new global::TimedState(sConnected, 6000, "Nombre maximum d'essais de regulation off atteint", sError);
                global::TimedState *sWriteRegulOn_FlankUp = new global::TimedState(sWriteRegulOn, 1000, "Réessayer la commande on regul flanc montant");
                global::TimedState *sWriteRegulOn_FlankDown = new global::TimedState(sWriteRegulOn, 1000, "Réessayer la commande on regul flanc descendent");
                global::TimedState *sVerifyRegulOn = new global::TimedState(sWriteRegulOn, 5000, "Echec de la vérification si le regul on", sError);

        // SuperState
        sSuperState->setInitialState(sDisconnected);
        QTimer *tPing = new QTimer(sSuperState);
        tPing->setInterval(ping_interval_);
        tPing->setSingleShot(false);
        tPing->start();
        QObject::connect(tPing, &QTimer::timeout, this, &Dee::Ping);

            // Disconnected
            auto tConnected = sDisconnected->addTransition(this, &Dee::SIGNAL_Connected, sConnected);
            QObject::connect(tConnected, &QSignalTransition::triggered, this, [&]() { DebugMessage("Connexion omron et adc établie"); });

            // Connected
            auto tDisconnected = sConnected->addTransition(this, &Dee::SIGNAL_Disconnected, sDisconnected);
            QObject::connect(tDisconnected, &QSignalTransition::triggered, this, [&]() { emit SIGNAL_Error(); });
            sConnected->setInitialState(sIdle);

                // Error
                QState::connect(sError, &QState::entered, this, &Dee::SIGNAL_Error);
                sError->addTransition(sError, &QState::entered, sIdle);

                // Idle
                auto tCmdRegulOn = sIdle->addTransition(this, &Dee::SIGNAL_CmdRegulationOn, sWriteRegulOn);
                QObject::connect(tCmdRegulOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande régulation on recu"); });
                auto tCmdRegulOff = sIdle->addTransition(this, &Dee::SIGNAL_CmdRegulationOff, sWriteRegulOff);
                QObject::connect(tCmdRegulOff, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande régulation off recu"); });

                // WriteRegulOff
                sWriteRegulOff->setInitialState(sWriteRegulOff_FlankUp);
                QObject::connect(sWriteRegulOff, &global::TimedState::SIGNAL_Timeout, this, &Dee::ErrorMessage);

                    // WriteRegulOff_FlankUp
                    QObject::connect(sWriteRegulOff_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Dee::WarningMessage);
                    QObject::connect(sWriteRegulOff_FlankUp, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CmdStringWriteRegulationOff(), true);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteRegulationOff()); });
                    });
                    sWriteRegulOff_FlankUp->addTransition(this, &Dee::SIGNAL_WriteRegulationOff_True, sWriteRegulOff_FlankDown);

                    // WriteRegulOff_FlankDown
                    QObject::connect(sWriteRegulOff_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Dee::WarningMessage);
                    QObject::connect(sWriteRegulOff_FlankDown, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CmdStringWriteRegulationOff(), false);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteRegulationOff()); });
                    });
                    auto tWriteRegulOff_Ok = sWriteRegulOff_FlankDown->addTransition(this, &Dee::SIGNAL_WriteRegulationOff_False, sVerifyRegulOff);
                    QObject::connect(tWriteRegulOff_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion regul off écrite"); });

                    // VerifyRegulOff
                    QObject::connect(sVerifyRegulOff, &global::TimedState::SIGNAL_Timeout, this, &Dee::ErrorMessage);
                    QObject::connect(sVerifyRegulOff, &QState::entered, this, &Dee::IsRegulationOn);
                    auto tRegulOff = sVerifyRegulOff->addTransition(this, &Dee::SIGNAL_RegulationOff, sIdle);
                    QObject::connect(tRegulOff, &QSignalTransition::triggered, this, [&]() { DebugMessage("Régulation off"); });

                // WriteRegulOn
                sWriteRegulOn->setInitialState(sWriteRegulOn_FlankUp);
                QObject::connect(sWriteRegulOn, &global::TimedState::SIGNAL_Timeout, this, &Dee::ErrorMessage);

                    // WriteRegulOn_FlankUp
                    QObject::connect(sWriteRegulOn_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &Dee::WarningMessage);
                    QObject::connect(sWriteRegulOn_FlankUp, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CmdStringWriteRegulationOn(), true);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteRegulationOn()); });
                    });
                    sWriteRegulOn_FlankUp->addTransition(this, &Dee::SIGNAL_WriteRegulationOn_True, sWriteRegulOn_FlankDown);

                    // WriteRegulOn_FlankDown
                    QObject::connect(sWriteRegulOn_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &Dee::WarningMessage);
                    QObject::connect(sWriteRegulOn_FlankDown, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CmdStringWriteRegulationOn(), false);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CmdStringWriteRegulationOn()); });
                    });
                    auto tWriteRegulOn_Ok = sWriteRegulOn_FlankDown->addTransition(this, &Dee::SIGNAL_WriteRegulationOn_False, sVerifyRegulOn);
                    QObject::connect(tWriteRegulOn_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion regul on écrite"); });

                    // VerifyRegulOn
                    QObject::connect(sVerifyRegulOn, &global::TimedState::SIGNAL_Timeout, this, &Dee::ErrorMessage);
                    QObject::connect(sVerifyRegulOn, &QState::entered, this, &Dee::IsRegulationOn);
                    auto tRegulOn = sVerifyRegulOn->addTransition(this, &Dee::SIGNAL_RegulationOn, sIdle);
                    QObject::connect(tRegulOn, &QSignalTransition::triggered, this, [&]() { DebugMessage("Régulation on"); });

             PrintStateChanges(sSuperState, "SuperState");
             PrintStateChanges(sDisconnected, "Disconnected");
             PrintStateChanges(sConnected, "Connected");
             PrintStateChanges(sError, "Error");
             PrintStateChanges(sIdle, "Idle");
             PrintStateChanges(sWriteRegulOff, "WriteRegulOff");
             PrintStateChanges(sWriteRegulOff_FlankUp, "WriteRegulOff_FlankUp");
             PrintStateChanges(sWriteRegulOff_FlankDown, "WriteRegulOff_FlankDown");
             PrintStateChanges(sVerifyRegulOff, "VerifyRegulOff");
             PrintStateChanges(sWriteRegulOn, "WriteRegulOn");
             PrintStateChanges(sWriteRegulOn_FlankUp, "WriteRegulOn_FlankUp");
             PrintStateChanges(sWriteRegulOn_FlankDown, "WriteRegulOn_FlankDown");
             PrintStateChanges(sVerifyRegulOn, "VerifyRegulOn");

             sm_.addState(sSuperState);
             sm_.setInitialState(sSuperState);
             sm_.start();
}

void Dee::CheckVoltage(double value) {
    if ((value - voltage_) > 0.2) {
        voltage_direction_++;
    } else if ((value - voltage_) < -0.2) {
        voltage_direction_--;
    }

    if (voltage_direction_ >= DIRECTION_COUNTS) {
        emit SIGNAL_VoltageIncreasing();
        voltage_direction_ = 0; // reset
    } else if (voltage_direction_ <= -DIRECTION_COUNTS) {
        emit SIGNAL_VoltageDecreasing();
        voltage_direction_ = 0; // reset
    } // else nothing
}

void Dee::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Dee " + GetHFBranch() + " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "Dee " + GetHFBranch() +" <-" << name; });
}

void Dee::InterpretADCChannelRead(const QString& channel, double value) {
   if (channel == CmdStringReadDeeVoltage()) {
       emit SIGNAL_DeeVoltage(value);
   } else if (channel == CmdStringReadDeePhase()) {
       emit SIGNAL_DeePhase(value);
   } // else nothing
}

void Dee::InterpretOmronBitRead(const QString& channel, bool content) {
    if (channel == CmdStringWriteRegulationOn()) {
        content ? emit SIGNAL_WriteRegulationOn_True() : emit SIGNAL_WriteRegulationOn_False();
    } else if (channel == CmdStringWriteRegulationOff()) {
        content ? emit SIGNAL_WriteRegulationOff_True() : emit SIGNAL_WriteRegulationOff_False();
    } else if (channel == CmdStringIsRegulationOn()) {
        content ? emit SIGNAL_RegulationOn() : emit SIGNAL_RegulationOff();
    } // else nothing
}

void Dee::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Dee::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Dee::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}

void Dee::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), GetHFBranch(), message);
}


} // namespace
