#include "RadiationMonitor.h"

#include <QFinalState>
#include <QEventTransition>
#include <QSignalTransition>
#include <QtDBus/QDBusConnection>
#include <memory>
#include <QTimer>
#include <unistd.h>
#include <QStandardPaths>

#include "ClientConnection.h"
#include "TimedState.h"
#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

RadiationMonitor::RadiationMonitor(Configuration configuration)
    : settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile), QSettings::IniFormat),
      configuration_comparator_(configuration),
      socket_(configuration.IP(),
              configuration.Port(),
              configuration.ConnectionTimeout(),
              configuration.ReadTimeout(),
              configuration.WriteTimeout(),
              QByteArray("\r\n")),
      check_configuration_(settings_.value(configuration.Name() + "/check_configuration", "True").toBool()),
      ring_buffer_(300),
      integrated_charge_measurement_enable_(settings_.value(configuration.Name() + "/integrated_charge_measurement", "False").toBool()),
      xray_identifier_(settings_.value(configuration.Name() + "/xray_identifier_enable", "False").toBool(),
                       settings_.value(configuration.Name() + "/xray_min_in_interval", "1").toInt(),
                       settings_.value(configuration.Name() + "/xray_max_in_interval", "3").toInt(),
                       settings_.value(configuration.Name() + "/xray_lower_interval", "0.8E-5").toDouble(),
                       settings_.value(configuration.Name() + "/xray_upper_interval", "8.0E-5").toDouble()),
      reconnect_interval_(settings_.value(configuration.Name() + "/reconnect_interval", "3000").toInt()),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "RadiationMonitor::RadiationMonitor settings";
    qDebug() << "Name " << configuration.Name();
    qDebug() << "IP " << configuration.IP();
    qDebug() << "Configuration enabled " << check_configuration_;
    qDebug() << "Integrated charge measurement enabled: " << integrated_charge_measurement_enable_;
    qDebug() << "X-ray identifier enabled: " << xray_identifier_.enabled();

    SetupCommandStateMachine();
    SetupConfiguringStateMachine();
    if (integrated_charge_measurement_enable_) {
        SetupChargeMeasurementStateMachine();
        SetupReadBufferStateMachine();
    }
    ConnectClientConnectionSignals();;
    ConnectReplyParserSignals();
    ConnectSignals();
//    socket_.Connect(); // TODO REMOVE AFTER TESTS
}

RadiationMonitor::~RadiationMonitor() {}

void RadiationMonitor::ConnectClientConnectionSignals() {
    QObject::connect(&socket_, &SocketClient::DataRead, &reply_parser_, qOverload<const QByteArray&>(&ReplyParser::Parse));
    QObject::connect(&socket_, &SocketClient::ReadError, this, [&](QString message) { DebugMessage(message); });
    QObject::connect(&socket_, &SocketClient::WriteError, this, [&](QString message) { DebugMessage(message); });
    QObject::connect(&socket_, &SocketClient::PortError, this, [&](QString message) { WarningMessage(message); });
    QObject::connect(&socket_, &SocketClient::ConnectionEstablished, this, &RadiationMonitor::SIGNAL_Connected);
    QObject::connect(&socket_, &SocketClient::Disconnected, this, &RadiationMonitor::SIGNAL_Disconnected);
}

void RadiationMonitor::ConnectSignals() {
    // Messaging
    QObject::connect(&reply_parser_, &ReplyParser::ReplyError, this, [&](QString msg){ DebugMessage("La balise a envoyé un message corrompu: " + msg); });
    QObject::connect(&reply_parser_, &ReplyParser::BaliseError, this, [&](QString msg) { DebugMessage("La balise signale une erreur interne: " + msg); });

    QObject::connect(this, &RadiationMonitor::InstantenousMeasurementChanged, this, [&](InstantenousMeasurement m){ emit SIGNAL_DoseRate(m.conversed_value()); });
    QObject::connect(this, &RadiationMonitor::InstantenousMeasurementChanged, &xray_identifier_, &XRayIdentifier::AddInstantaneousValue);
    QObject::connect(&xray_identifier_, &XRayIdentifier::XRayIdentified, this, &RadiationMonitor::HandleDetectedXRay);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1Changed, this, [&](IntegratedMeasurement m) { emit SIGNAL_IntegratedChargeContinous(m.charge()); });
}

void RadiationMonitor::ConnectReplyParserSignals() {
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementChanged, this, &RadiationMonitor::InstantenousMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementChanged, this, [&](medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::InstantenousMeasurement m) {
        qDebug() << "InstantenousMeasurement " << m.timestamp() << " " << m.conversed_value(); });
    QObject::connect(&reply_parser_, &ReplyParser::CommandConfirmed, this, [=]() { configuration_comparator_.CommandConfirmationChanged(true); });
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1Changed, this, &RadiationMonitor::AddIntegratedToRingBuffer);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1Changed, this, &RadiationMonitor::IntegratedMeasurement1Changed);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2Changed, this, &RadiationMonitor::IntegratedMeasurement2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement1Changed, this, &RadiationMonitor::AddBufferedToRingBuffer);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement1Changed, this, &RadiationMonitor::BufferIntegratedMeasurement1Changed);
    QObject::connect(&reply_parser_, &ReplyParser::BufferIntegratedMeasurement2Changed, this, &RadiationMonitor::BufferIntegratedMeasurement2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::ThresholdStatusChanged, this, &RadiationMonitor::ThresholdStatusChanged);
    QObject::connect(&reply_parser_, &ReplyParser::ModeFunctionnementChanged, this, &RadiationMonitor::ModeFunctionnementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::ModeFunctionnementChanged, &configuration_comparator_, &ConfigurationComparator::ModeFunctionnementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliTypeChanged, this, &RadiationMonitor::PreAmpliTypeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliTypeChanged, &configuration_comparator_, &ConfigurationComparator::PreAmpliTypeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliGammeChanged, this, &RadiationMonitor::PreAmpliGammeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::PreAmpliGammeChanged, &configuration_comparator_, &ConfigurationComparator::PreAmpliGammeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitInstantenousMeasurementChanged, this, &RadiationMonitor::EmitInstantenousMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitInstantenousMeasurementChanged, &configuration_comparator_, &ConfigurationComparator::InstantaneousLAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated1MeasurementChanged, this, &RadiationMonitor::EmitIntegrated1MeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated1MeasurementChanged, &configuration_comparator_, &ConfigurationComparator::Integrated1LAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated2MeasurementChanged, this, &RadiationMonitor::EmitIntegrated2MeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::EmitIntegrated2MeasurementChanged, &configuration_comparator_, &ConfigurationComparator::Integrated2LAMChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeDateInMeasurementChanged, this, &RadiationMonitor::IncludeDateInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeDateInMeasurementChanged, &configuration_comparator_, &ConfigurationComparator::IncludeDateInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeConversedValueInMeasurementChanged, this, &RadiationMonitor::IncludeConversedValueInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IncludeConversedValueInMeasurementChanged, &configuration_comparator_, &ConfigurationComparator::IncludeRawInMeasurementChanged);
    QObject::connect(&reply_parser_, &ReplyParser::DateChanged, this, &RadiationMonitor::DateChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementTimeChanged, this, &RadiationMonitor::InstantenousMeasurementTimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementTimeChanged, &configuration_comparator_, &ConfigurationComparator::InstantaneousTimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementElementsChanged, this, &RadiationMonitor::InstantenousMeasurementElementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementElementsChanged, &configuration_comparator_, &ConfigurationComparator::InstantaneousElementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsBelowThresholdBeforeAlarmResetChanged, this, &RadiationMonitor::NmbMeasurementsBelowThresholdBeforeAlarmResetChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1TimeChanged, this, &RadiationMonitor::IntegratedMeasurement1TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement1TimeChanged, &configuration_comparator_, &ConfigurationComparator::Integrated1TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2TimeChanged, this, &RadiationMonitor::IntegratedMeasurement2TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurement2TimeChanged, &configuration_comparator_, &ConfigurationComparator::Integrated2TimeChanged);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsIntegrationTime2Changed, this, &RadiationMonitor::NmbMeasurementsIntegrationTime2Changed);
    QObject::connect(&reply_parser_, &ReplyParser::NmbMeasurementsIntegrationTime2Changed, &configuration_comparator_, &ConfigurationComparator::NumberOfMeasurementsChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, this, &RadiationMonitor::InstantenousMeasurementConversionCoeffChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, &configuration_comparator_, &ConfigurationComparator::InstantaneousConversionCoefficientChanged);
    QObject::connect(&reply_parser_, &ReplyParser::InstantenousMeasurementConversionCoeffChanged, this,
                     [=](double coeff) { inst_conversion_coeff_value_ = coeff; inst_conversion_coeff_init_ = true; });
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementConversionCoeffChanged, this, &RadiationMonitor::IntegratedMeasurementConversionCoeffChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementConversionCoeffChanged, &configuration_comparator_, &ConfigurationComparator::IntegratedConversionCoefficientChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementThresholdLevelChanged, this, &RadiationMonitor::IntegratedMeasurementThresholdLevelChanged);
    QObject::connect(&reply_parser_, &ReplyParser::IntegratedMeasurementThresholdLevelChanged, &configuration_comparator_, &ConfigurationComparator::IntegratedThresholdChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold1Changed, &configuration_comparator_, &ConfigurationComparator::ThresholdAChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold2Changed, &configuration_comparator_, &ConfigurationComparator::ThresholdBChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold3Changed, &configuration_comparator_, &ConfigurationComparator::ThresholdCChanged);
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold1Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit WarningThreshold1Changed(ampere * inst_conversion_coeff_value_); });
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold2Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit WarningThreshold2Changed(ampere * inst_conversion_coeff_value_); });
    QObject::connect(&reply_parser_, &ReplyParser::WarningThreshold3Changed, this, [=](double ampere) {
        if (inst_conversion_coeff_init_) emit WarningThreshold3Changed(ampere * inst_conversion_coeff_value_); });
}

void RadiationMonitor::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("%1").arg(configuration_comparator_.DesiredConfiguration().Name()), message);
}

void RadiationMonitor::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("%1").arg(configuration_comparator_.DesiredConfiguration().Name()), message);
}

void RadiationMonitor::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("%1").arg(configuration_comparator_.DesiredConfiguration().Name()), message);
}

void RadiationMonitor::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("%1").arg(configuration_comparator_.DesiredConfiguration().Name()), message);
}

void RadiationMonitor::EnableRemoteCommunication() {
    DebugMessage("Activer la communication à distance");
    command_list_.push_back(command_creator_.EnableRemote(false));
    command_list_.push_back(command_creator_.StopMeasurement(false));
    command_list_.push_back(command_creator_.EnableReply());
    // Date used as a signal that remote communication has been successfully established
    command_list_.push_back(command_creator_.SetDate(QDateTime::currentDateTime()));
    command_list_.push_back(command_creator_.ReadDate());
    emit SIGNAL_CommandAdded();
}

void RadiationMonitor::ReadConfiguration() {
    DebugMessage("Lire la configuration actuelle");
     command_list_.push_back(command_creator_.ReadNumericParameters());
     command_list_.push_back(command_creator_.ReadModeParameters());
     emit SIGNAL_CommandAdded();
}

void RadiationMonitor::WriteConfiguration() {
    DebugMessage("Envoyer la configuration");
    if (configuration_comparator_.IsInitialised()) {
        if (!configuration_comparator_.ModeFunctionnementCorrect()) command_list_.push_back(command_creator_.SetModeFunctionnement(configuration_comparator_.GetCorrectModeFunctionnement()));
        if (!configuration_comparator_.PreAmpliTypeCorrect()) command_list_.push_back(command_creator_.SetPreAmpliType(configuration_comparator_.GetCorrectPreAmpliType()));
        if (!configuration_comparator_.PreAmpliGammeCorrect()) command_list_.push_back(command_creator_.SetPreAmpliGamme(configuration_comparator_.GetCorrectPreAmpliGamme()));
        if (!configuration_comparator_.InstantaneousLAMCorrect()) command_list_.push_back(command_creator_.SetEmitInstanenousMeasurement(configuration_comparator_.GetCorrectInstantaneousLAM()));
        if (!configuration_comparator_.Integrated1LAMCorrect()) command_list_.push_back(command_creator_.SetEmitIntegrated1Measurement(configuration_comparator_.GetCorrectIntegrated1LAM()));
        if (!configuration_comparator_.Integrated2LAMCorrect()) command_list_.push_back(command_creator_.SetEmitIntegrated2Measurement(configuration_comparator_.GetCorrectIntegrated2LAM()));
        if (!configuration_comparator_.IncludeDateInMeasurementCorrect()) command_list_.push_back(command_creator_.SetIncludeDateInReply(configuration_comparator_.GetCorrectIncludeDateInMeasurement()));
        if (!configuration_comparator_.IncludeRawInMeasurementCorrect()) command_list_.push_back(command_creator_.SetIncludeRawMeasurementInReply(configuration_comparator_.GetCorrectIncludeRawInMeasurement()));
        if (!configuration_comparator_.InstantaneousElementsCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementElements(configuration_comparator_.GetCorrectInstantaneousElements()));
        if (!configuration_comparator_.InstantaneousTimeCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementTime(configuration_comparator_.GetCorrectInstantaneousTime()));
        if (!configuration_comparator_.Integrated1TimeCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurement1Time(configuration_comparator_.GetCorrectIntegrated1Time()));
        if (!configuration_comparator_.Integrated2TimeCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurement2Time(configuration_comparator_.GetCorrectIntegrated2Time()));
        if (!configuration_comparator_.NumberOfMeasurementsCorrect()) command_list_.push_back(command_creator_.SetNmbMeasurementsIntegrationTime2(configuration_comparator_.GetCorrectNumberOfMeasurements()));
//        if (!configuration_comparator_.InstantaneousConversionCoefficientCorrect()) command_list_.push_back(command_creator_.SetInstantenousMeasurementConversionCoefficient(configuration_comparator_.GetCorrectInstantaneousConversionCoefficient()));
//        if (!configuration_comparator_.IntegratedConversionCoefficientCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurementConversionCoefficient(configuration_comparator_.GetCorrectIntegratedConversionCoefficient()));
//        if (!configuration_comparator_.IntegratedThresholdCorrect()) command_list_.push_back(command_creator_.SetIntegratedMeasurementThresholdLevel(configuration_comparator_.GetCorrectIntegratedThreshold()));
//        if (!configuration_comparator_.ThresholdACorrect()) command_list_.push_back(command_creator_.SetWarningThreshold1(configuration_comparator_.GetCorrectThresholdA()));
//        if (!configuration_comparator_.ThresholdBCorrect()) command_list_.push_back(command_creator_.SetWarningThreshold2(configuration_comparator_.GetCorrectThresholdB()));
//        if (!configuration_comparator_.ThresholdCCorrect()) command_list_.push_back(command_creator_.SetWarningThreshold3(configuration_comparator_.GetCorrectThresholdC()));
    }
    command_list_.push_back(command_creator_.ReadNumericParameters());
    command_list_.push_back(command_creator_.ReadModeParameters());
    emit SIGNAL_CommandAdded();
}

void RadiationMonitor::SendNextCommand() {
    if (command_list_.empty()) {
        return;
    }
    Command cmd(command_list_.takeFirst());
    qDebug() << "RadiationMonitor::SendNextCommand " << cmd.ascii();
    cmd.Send(&socket_);
    if (!cmd.wait_confirmation()) {
        emit SIGNAL_CommandSentNoReply();
    } else if (!cmd.wait_reply()) {
        emit SIGNAL_CommandSentConfirmation();
    } else {
        emit SIGNAL_CommandSentReply();
    }
}

void RadiationMonitor::SetupCommandStateMachine() {
    QState *sCommandSuperState = new QState();
        QState *sBrokenConnection = new QState(sCommandSuperState);
        global::TimedState *sDisconnected = new global::TimedState(sCommandSuperState, reconnect_interval_, "Reconnecting");
        QState *sConnected = new QState(sCommandSuperState);
            QState *sIdle = new QState(sConnected);
            QState *sWaitingNoReply = new QState(sConnected);
            QState *sWaitingConfirmation = new QState(sConnected);
            QState *sWaitingReply = new QState(sConnected);
            QState *sDelay = new QState(sConnected);

//    QObject::connect(sIdle, &QState::entered, this, [=]() { CommandStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { CommandStateMachine("Idle exited"); });
//    QObject::connect(sWaitingNoReply, &QState::entered, this, [=]() { CommandStateMachine("WaitingNoReply entered"); });
//    QObject::connect(sWaitingNoReply, &QState::exited, this, [=]() { CommandStateMachine("WaitingNoReply exited"); });
//    QObject::connect(sWaitingConfirmation, &QState::entered, this, [=]() { CommandStateMachine("WaitingConfirmation entered"); });
//    QObject::connect(sWaitingConfirmation, &QState::exited, this, [=]() { CommandStateMachine("WaitingConfirmation exited"); });
//    QObject::connect(sWaitingReply, &QState::entered, this, [=]() { CommandStateMachine("WaitingReply entered"); });
//    QObject::connect(sWaitingReply, &QState::exited, this, [=]() { CommandStateMachine("WaitingReply exited"); });
//    QObject::connect(sError, &QState::entered, this, [=]() { CommandStateMachine("Error entered"); });
//    QObject::connect(sError, &QState::exited, this, [=]() { CommandStateMachine("Error exited"); });
//    QObject::connect(sCommandSuperState, &QState::entered, this, [=]() { CommandStateMachine("CommandSuperState entered"); });
//    QObject::connect(sCommandSuperState, &QState::exited, this, [=]() { CommandStateMachine("CommandSuperState exited"); });

    // SuperState
    sCommandSuperState->setInitialState(sDisconnected);
    QObject::connect(&socket_, &SocketClient::Disconnected, this, [&]() { QTimer::singleShot(reconnect_interval_, this, [&]() { emit SIGNAL_Reconnect(); }); });
    sCommandSuperState->addTransition(this, &RadiationMonitor::SIGNAL_Reconnect, sCommandSuperState);

        // Broken connection
        QObject::connect(sBrokenConnection, &QState::entered, &socket_, &SocketClient::Disconnect);
        QObject::connect(sBrokenConnection, &QState::entered, this, [&]() { ErrorMessage("Débranché. Reconnexion.."); });

        // Disconnected
        QObject::connect(sDisconnected, &QState::entered, this, &RadiationMonitor::EmptyCommandQueue);
        QObject::connect(sDisconnected, &global::TimedState::SIGNAL_Timeout, this, [&]() { socket_.Connect(); });
        sDisconnected->addTransition(this, &RadiationMonitor::SIGNAL_Connected, sConnected);

        // Connected
        sConnected->addTransition(this, &RadiationMonitor::SIGNAL_Disconnected, sDisconnected);
        QObject::connect(sConnected, &QState::entered, this, [&]() { DebugMessage("Connecté"); });
        sConnected->setInitialState(sIdle);

            // Idle
            sIdle->addTransition(this, &RadiationMonitor::SIGNAL_CommandAdded, sIdle); // self transition
            sIdle->addTransition(this, &RadiationMonitor::SIGNAL_CommandSentNoReply, sWaitingNoReply);
            sIdle->addTransition(this, &RadiationMonitor::SIGNAL_CommandSentConfirmation, sWaitingConfirmation);
            sIdle->addTransition(this, &RadiationMonitor::SIGNAL_CommandSentReply, sWaitingReply);
            QObject::connect(sIdle, &QState::entered, this, &RadiationMonitor::SendNextCommand);

    QTimer *noReplyTimer = new QTimer(sWaitingNoReply);
    noReplyTimer->setInterval(3000);
    noReplyTimer->setSingleShot(true);
    QObject::connect(sWaitingNoReply, SIGNAL(entered()), noReplyTimer, SLOT(start()));
    QObject::connect(sWaitingNoReply, SIGNAL(exited()), noReplyTimer, SLOT(stop()));
    sWaitingNoReply->addTransition(noReplyTimer, SIGNAL(timeout()), sIdle);

    QTimer *confirmTimer = new QTimer(sWaitingConfirmation);
    confirmTimer->setInterval(2000);
    confirmTimer->setSingleShot(true);
    QObject::connect(sWaitingConfirmation, SIGNAL(entered()), confirmTimer, SLOT(start()));
    QObject::connect(sWaitingConfirmation, SIGNAL(exited()), confirmTimer, SLOT(stop()));
    QObject::connect(confirmTimer, &QTimer::timeout, this, [this]() { DebugMessage("Timeout de la confirmation de la commande"); });
    sWaitingConfirmation->addTransition(confirmTimer, SIGNAL(timeout()), sIdle);
    sWaitingConfirmation->addTransition(&reply_parser_, &ReplyParser::CommandConfirmed, sDelay);

    QTimer *replyTimer = new QTimer(sWaitingReply);
    replyTimer->setInterval(3000);
    replyTimer->setSingleShot(true);
    QObject::connect(sWaitingReply, SIGNAL(entered()), replyTimer, SLOT(start()));
    QObject::connect(sWaitingReply, SIGNAL(exited()), replyTimer, SLOT(stop()));
    QObject::connect(replyTimer, &QTimer::timeout, this, [this]() { DebugMessage("Timeout de la réponse à la commande"); });
    sWaitingReply->addTransition(replyTimer, SIGNAL(timeout()), sIdle);
    sWaitingReply->addTransition(&reply_parser_, &ReplyParser::CommandReply, sDelay);

    QTimer *delayTimer = new QTimer(sDelay);
    delayTimer->setInterval(300);
    delayTimer->setSingleShot(true);
    QObject::connect(sDelay, SIGNAL(entered()), delayTimer, SLOT(start()));
    sDelay->addTransition(delayTimer, SIGNAL(timeout()), sIdle);

    command_sm_.addState(sCommandSuperState);
    command_sm_.setInitialState(sCommandSuperState);
    command_sm_.start();
}

void RadiationMonitor::SetupConfiguringStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        global::TimedState *sInitialize = new global::TimedState(sSuperState, 60000, "Echec de l'initialisation de la balise, arrêter", sIdle);
            global::TimedState *sEnableRemote = new global::TimedState(sInitialize, 10000, "Echec de l'activation de la communication à distance");
            global::TimedState *sCheckConfiguration = new global::TimedState(sInitialize, 20000, "Echec de la lecture de la configuration");
            global::TimedState *sConfigure = new global::TimedState(sInitialize, 30000, "Echec de l'écriture de la configuration");
            global::TimedState *sStartMeasurement = new global::TimedState(sInitialize, 10000, "Echec du démarrage des mesures");
        global::TimedState *sReady = new global::TimedState(sSuperState, 10000, "La balise a cessé d'envoyer des mesures, reconfiguration du controlleur", sInitialize);

    QObject::connect(sSuperState, &QState::entered, this, [=]() { ConfiguringStateMachine("SuperState entered"); });
    QObject::connect(sSuperState, &QState::exited, this, [=]() { ConfiguringStateMachine("SuperState exited"); });
    QObject::connect(sIdle, &QState::entered, this, [=]() { ConfiguringStateMachine("Idle entered"); });
    QObject::connect(sIdle, &QState::exited, this, [=]() { ConfiguringStateMachine("Idle exited"); });
    QObject::connect(sInitialize, &QState::entered, this, [=]() { ConfiguringStateMachine("Initialize entered"); });
    QObject::connect(sInitialize, &QState::exited, this, [=]() { ConfiguringStateMachine("Initialize exited"); });
    QObject::connect(sEnableRemote, &QState::entered, this, [=]() { ConfiguringStateMachine("EnableRemote entered"); });
    QObject::connect(sEnableRemote, &QState::exited, this, [=]() { ConfiguringStateMachine("EnableRemote exited"); });
    QObject::connect(sCheckConfiguration, &QState::entered, this, [=]() { ConfiguringStateMachine("CheckConfiguration entered"); });
    QObject::connect(sCheckConfiguration, &QState::exited, this, [=]() { ConfiguringStateMachine("CheckConfiguration exited"); });
    QObject::connect(sConfigure, &QState::entered, this, [=]() { ConfiguringStateMachine("Configure entered"); });
    QObject::connect(sConfigure, &QState::exited, this, [=]() { ConfiguringStateMachine("Configure exited"); });
    QObject::connect(sStartMeasurement, &QState::entered, this, [=]() { ConfiguringStateMachine("StartMeasurement entered"); });
    QObject::connect(sStartMeasurement, &QState::exited, this, [=]() { ConfiguringStateMachine("StartMeasurement exited"); });
//    QObject::connect(sReady, &QState::entered, this, [=]() { ConfiguringStateMachine("Ready entered"); });
//    QObject::connect(sReady, &QState::exited, this, [=]() { ConfiguringStateMachine("Ready exited"); });

    sSuperState->setInitialState(sInitialize);
    sSuperState->addTransition(this, &RadiationMonitor::SIGNAL_StartConfiguration, sInitialize);

    // Idle
    // No actions

    // Initialize
    sInitialize->setInitialState(sEnableRemote);
    QObject::connect(sInitialize, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { ErrorMessage(message); });

    // EnableRemote
    QObject::connect(sEnableRemote, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(message); });
    QObject::connect(sEnableRemote, &QState::entered, this, &RadiationMonitor::EmptyCommandQueue);
    QObject::connect(sEnableRemote, &QState::entered, this, &RadiationMonitor::EnableRemoteCommunication);
    if (check_configuration_) {
        sEnableRemote->addTransition(&reply_parser_, &ReplyParser::DateChanged, sCheckConfiguration);
    } else {
        sEnableRemote->addTransition(&reply_parser_, &ReplyParser::DateChanged, sStartMeasurement);
    }

    // CheckConfiguration
    QObject::connect(sCheckConfiguration, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(message); });
    QObject::connect(sCheckConfiguration, &QState::entered, this, &RadiationMonitor::EmptyCommandQueue);
    QObject::connect(sCheckConfiguration, &QState::entered, &configuration_comparator_, &ConfigurationComparator::ResetStatus);
    QObject::connect(sCheckConfiguration, &QState::entered, this, &RadiationMonitor::ReadConfiguration);
    sCheckConfiguration->addTransition(&configuration_comparator_, &ConfigurationComparator::Ok, sStartMeasurement);
    sCheckConfiguration->addTransition(&configuration_comparator_, &ConfigurationComparator::Wrong, sConfigure);

    // Configure
    QObject::connect(sConfigure, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { WarningMessage(message); });
    QObject::connect(sConfigure, &QState::entered, this, &RadiationMonitor::EmptyCommandQueue);
    QObject::connect(sConfigure, &QState::entered, this, &RadiationMonitor::WriteConfiguration);
    auto tConfigure = sConfigure->addTransition(&configuration_comparator_, &ConfigurationComparator::Ok, sStartMeasurement);
    QObject::connect(tConfigure, &QSignalTransition::triggered, this, [&]() { DebugMessage("Balise configurée avec succès"); });

    // StartMeasurement
    QObject::connect(sStartMeasurement, &QState::entered, this, &RadiationMonitor::EmptyCommandQueue);
    QObject::connect(sStartMeasurement, &QState::entered, this, &RadiationMonitor::StartMeasurement);
    auto tStartMeasurement = sStartMeasurement->addTransition(&reply_parser_, &ReplyParser::InstantenousMeasurementChanged, sReady);
    QObject::connect(tStartMeasurement, &QSignalTransition::triggered, this, [&]() { InfoMessage("Les mesures ont commencé"); });

    // Ready
    QObject::connect(sReady, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { ErrorMessage(message); });
    if (integrated_charge_measurement_enable_) {
        QObject::connect(sReady, &QState::entered, this, &RadiationMonitor::ReadIntegratedMeasurement1);
    }
    sReady->addTransition(this, &RadiationMonitor::InstantenousMeasurementChanged, sReady);

    configuring_sm_.addState(sSuperState);
    configuring_sm_.setInitialState(sSuperState);
    configuring_sm_.start();
}

void RadiationMonitor::SetupChargeMeasurementStateMachine() {
    QState *sIdle = new QState();
    QState *sMeasurement = new QState();
        QState *sStart = new QState(sMeasurement);
        QState *sInProgress = new QState(sMeasurement);
         QState *sStop = new QState(sMeasurement);
            QState *sDelay = new QState(sStop);
            QState *sSave = new QState(sStop);

//    QObject::connect(sMeasurement, &QState::entered, this, [=]() { MeasurementStateMachine("MeasurementState entered"); });
//    QObject::connect(sMeasurement, &QState::exited, this, [=]() { MeasurementStateMachine("MeasurementState exited"); });
//    QObject::connect(sIdle, &QState::entered, this, [=]() { MeasurementStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { MeasurementStateMachine("Idle exited"); });
//    QObject::connect(sStart, &QState::entered, this, [=]() { MeasurementStateMachine("Start entered"); });
//    QObject::connect(sStart, &QState::exited, this, [=]() { MeasurementStateMachine("Start exited"); });
//    QObject::connect(sStop, &QState::entered, this, [=]() { MeasurementStateMachine("Stop entered"); });
//    QObject::connect(sStop, &QState::exited, this, [=]() { MeasurementStateMachine("Stop exited"); });
//    QObject::connect(sSave, &QState::entered, this, [=]() { MeasurementStateMachine("Save entered"); });
//    QObject::connect(sSave, &QState::exited, this, [=]() { MeasurementStateMachine("Save exited"); });
//    QObject::connect(sDelay, &QState::entered, this, [=]() { MeasurementStateMachine("Delay entered"); });
//    QObject::connect(sDelay, &QState::exited, this, [=]() { MeasurementStateMachine("Delay exited"); });

    // IDLE
    sIdle->addTransition(this, &RadiationMonitor::SIGNAL_StartChargeMeasurement, sMeasurement);

    // MEASUREMENT SUPER STATE
    QObject::connect(sMeasurement, &QState::entered, this, [=]() { DebugMessage("Démarrage de la mesure de charge"); });
    sMeasurement->setInitialState(sStart);
    sMeasurement->addTransition(this, &RadiationMonitor::SIGNAL_StartConfiguration, sIdle);
    sMeasurement->addTransition(this, &RadiationMonitor::SIGNAL_AbortChargeMeasurement, sIdle);

    // START STATE
    QObject::connect(sStart, &QState::entered, this, [=]() {
        try {
            charge_measurement_params_.start = ring_buffer_.CurrentTime().addSecs(-2);
        } catch(std::exception& exc) {
            emit SIGNAL_AbortChargeMeasurement();
            WarningMessage("Mesure de charge interrompue, aucune mesure intégrée disponible");
        } });
    sStart->addTransition(sStart, &QState::entered, sInProgress);

    // INPROGRESS STATE
    QTimer *inProgressTimer = new QTimer(sInProgress);
    QObject::connect(inProgressTimer, &QTimer::timeout, this, [=]() { WarningMessage("Timeout de mesure de charge après 10 min"); });
    inProgressTimer->setInterval(1000 * 10 * 60); // 10 min measurements is max
    inProgressTimer->setSingleShot(true);
    QObject::connect(sInProgress, SIGNAL(entered()), inProgressTimer, SLOT(start()));
    QObject::connect(sInProgress, &QState::exited, inProgressTimer, &QTimer::stop);
    sInProgress->addTransition(inProgressTimer, SIGNAL(timeout()), sIdle);
    sInProgress->addTransition(this, &RadiationMonitor::SIGNAL_StopChargeMeasurement, sStop);
    QObject::connect(sInProgress, &QState::entered, this, &RadiationMonitor::SIGNAL_MeasurementInProgress);

    // STOP SUPERSTATE
    QObject::connect(sStop, &QState::entered, this, [=]() { DebugMessage("Arrêt de la mesure de charge"); });
    sStop->setInitialState(sDelay);
    QTimer *delayTimer = new QTimer(sDelay);
    delayTimer->setInterval(3000);
    delayTimer->setSingleShot(true);
    QObject::connect(sDelay, SIGNAL(entered()), delayTimer, SLOT(start()));
    sDelay->addTransition(delayTimer, SIGNAL(timeout()), sSave);
    // Waiting for the next integrated measurement to arrive, then stop
    sDelay->addTransition(this, &RadiationMonitor::IntegratedMeasurement1Changed, sSave);
    QObject::connect(sDelay, &QState::exited, this, [=]() { charge_measurement_params_.stop = ring_buffer_.CurrentTime(); });
    QObject::connect(sDelay, &QState::exited, this, &RadiationMonitor::SIGNAL_MeasurementStopped);

    QObject::connect(sSave, &QState::entered, this, &RadiationMonitor::BroadcastChargeMeasurement);
    sSave->addTransition(sSave, &QState::entered, sIdle);

    charge_measurement_sm_.addState(sIdle);
    charge_measurement_sm_.addState(sMeasurement);
    charge_measurement_sm_.setInitialState(sIdle);
    charge_measurement_sm_.start();
}

void RadiationMonitor::SetupReadBufferStateMachine() {
    QState *sIdle = new QState();
    QState *sReadBuffer = new QState();

//    QObject::connect(sIdle, &QState::entered, this, [=]() { ReadBufferStateMachine("Idle entered"); });
//    QObject::connect(sIdle, &QState::exited, this, [=]() { ReadBufferStateMachine("Idle exited"); });
//    QObject::connect(sReadBuffer, &QState::entered, this, [=]() { ReadBufferStateMachine("ReadBuffer entered"); });
//    QObject::connect(sReadBuffer, &QState::exited, this, [=]() { ReadBufferStateMachine("ReadBuffer exited"); });

    // IDLE
    QObject::connect(sIdle, &QState::entered, this, [=]() { n_buffer_read_tries_ = 0; });
    sIdle->addTransition(&ring_buffer_, &MeasurementRingBuffer::IntegratedChargeDecreased, sReadBuffer);

    // READ BUFFER
    QObject::connect(sReadBuffer, &QState::entered, this, [=]() { n_buffer_read_tries_++; });
    QObject::connect(sReadBuffer, &QState::entered, this, [=]() {
        if (n_buffer_read_tries_ <= 10) {
            ReadBufferIntegratedMeasurement1();
        } else {
            WarningMessage("Échec de la lecture du buffer intégré");
            emit SIGNAL_FailedSecureReadIntegratedBuffer();
        }
    });
    sReadBuffer->addTransition(this, &RadiationMonitor::SIGNAL_FailedSecureReadIntegratedBuffer, sIdle);

    QTimer *readBufferTimer = new QTimer(sReadBuffer);
    readBufferTimer->setInterval(2000);
    readBufferTimer->setSingleShot(true);
    QObject::connect(sReadBuffer, SIGNAL(entered()), readBufferTimer, SLOT(start()));
    QObject::connect(sReadBuffer, SIGNAL(exited()), readBufferTimer, SLOT(stop()));
    sReadBuffer->addTransition(readBufferTimer, SIGNAL(timeout()), sReadBuffer);
    sReadBuffer->addTransition(this, &RadiationMonitor::BufferIntegratedMeasurement1Changed, sIdle);

    readBuffer_sm_.addState(sIdle);
    readBuffer_sm_.addState(sReadBuffer);
    readBuffer_sm_.setInitialState(sIdle);
    readBuffer_sm_.start();
}

double RadiationMonitor::GetIntegratedCharge(QDateTime time1, QDateTime time2) {
    try {
        return ring_buffer_.GetIntegratedCharge(time1, time2);
    }
    catch (std::exception& exc) {
        qDebug() << "RadiationMonitor::GetIntegratedCharge Exception thrown " << exc.what();
        QString msg = "Impossible de calculer la charge intégrée entre " + time1.toString() + " et " + time2.toString();
        WarningMessage(msg);
        return 0.0;
    }
}

void RadiationMonitor::BroadcastChargeMeasurement() {
    qDebug() << "RadiationMonitor::BroadcastChargeMeasurement: start time " << charge_measurement_params_.start << " stop time " << charge_measurement_params_.stop;
    const double integrated_charge = GetIntegratedCharge(charge_measurement_params_.start, charge_measurement_params_.stop);
    qDebug() << "RadiationMonitor::BroadcastMeasurement Integrated charge " << integrated_charge;
    emit SIGNAL_IntegratedChargeOverInterval(integrated_charge);
}

void RadiationMonitor::AddCommand(Command command, bool force) {
    if (static_cast<int>(command_list_.size()) <= 1 || force) {
        command_list_.push_back(command);
        emit SIGNAL_CommandAdded();
    } else {
        qWarning() << "RadiationMonitor::AddCommand There are already" << command_list_.size() << " commands in queue, skipping this one";
    }
}

void RadiationMonitor::ForceAddCommand(Command command) {
    command_list_.push_back(command);
    emit SIGNAL_CommandAdded();
}

void RadiationMonitor::PowerOn() { AddCommand(command_creator_.PowerOn()); }
void RadiationMonitor::StartMeasurement() { AddCommand(command_creator_.StartMeasurement()); }
void RadiationMonitor::StopMeasurement() { AddCommand(command_creator_.StopMeasurement(true)); }
void RadiationMonitor::SetSeparator(QString separator) { AddCommand(command_creator_.SetSeparator(separator)); }
void RadiationMonitor::SetSeparator() { SetSeparator(cmd_separator_); }
void RadiationMonitor::EnableRemote(bool wait_confirmation) { AddCommand(command_creator_.EnableRemote(wait_confirmation)); }
void RadiationMonitor::EnableLocal() { AddCommand(command_creator_.EnableLocal()); }
void RadiationMonitor::EnableReply() { AddCommand(command_creator_.EnableReply()); }
void RadiationMonitor::DisableReply() { AddCommand(command_creator_.DisableReply()); }

void RadiationMonitor::SetModeFunctionnement(ModeFunctionnement mode) { AddCommand(command_creator_.SetModeFunctionnement(mode)); }
void RadiationMonitor::SetPreAmpliType(PreAmpliType type) { AddCommand(command_creator_.SetPreAmpliType(type)); }
void RadiationMonitor::SetPreAmpliGamme(PreAmpliGamme gamme) { AddCommand(command_creator_.SetPreAmpliGamme(gamme)); }
void RadiationMonitor::SetEmitInstanenousMeasurement(bool yes) { AddCommand(command_creator_.SetEmitInstanenousMeasurement(yes)); }
void RadiationMonitor::SetEmitIntegrated1Measurement(bool yes) { AddCommand(command_creator_.SetEmitIntegrated1Measurement(yes)); }
void RadiationMonitor::SetEmitIntegrated2Measurement(bool yes) { AddCommand(command_creator_.SetEmitIntegrated2Measurement(yes)); }
void RadiationMonitor::SetIncludeDateInReply(bool yes) { AddCommand(command_creator_.SetIncludeDateInReply(yes)); }
void RadiationMonitor::SetIncludeRawMeasurementInReply(bool yes) { AddCommand(command_creator_.SetIncludeRawMeasurementInReply(yes)); }

void RadiationMonitor::SetInstantenousMeasurementTime(int seconds) { AddCommand(command_creator_.SetInstantenousMeasurementTime(seconds)); }
void RadiationMonitor::SetInstantenousMeasurementElements(int elements) { AddCommand(command_creator_.SetInstantenousMeasurementElements(elements)); }
void RadiationMonitor::SetNmbMeasurementsBelowThresholdBeforeAlarmReset(int measurements) { AddCommand(command_creator_.SetNmbMeasurementsBelowThresholdBeforeAlarmReset(measurements)); }
void RadiationMonitor::SetIntegratedMeasurement1Time(int minutes) { AddCommand(command_creator_.SetIntegratedMeasurement1Time(minutes)); }
void RadiationMonitor::SetIntegratedMeasurement2Time(int hours) { AddCommand(command_creator_.SetIntegratedMeasurement2Time(hours)); }
void RadiationMonitor::SetNmbMeasurementsIntegrationTime2(int measurements) { AddCommand(command_creator_.SetNmbMeasurementsIntegrationTime2(measurements)); }
void RadiationMonitor::SetInstantenousMeasurementConversionCoefficient(double coeff) { AddCommand(command_creator_.SetInstantenousMeasurementConversionCoefficient(coeff)); }
void RadiationMonitor::SetIntegratedMeasurementConversionCoefficient(double coeff) { AddCommand(command_creator_.SetIntegratedMeasurementConversionCoefficient(coeff)); }
void RadiationMonitor::SetIntegratedMeasurementThresholdLevel(double ampere) { AddCommand(command_creator_.SetIntegratedMeasurementThresholdLevel(ampere)); }
void RadiationMonitor::SetWarningThreshold1(double conv_value) { AddCommand(command_creator_.SetWarningThreshold1(conv_value)); }
void RadiationMonitor::SetWarningThreshold2(double conv_value) { AddCommand(command_creator_.SetWarningThreshold2(conv_value)); }
void RadiationMonitor::SetWarningThreshold3(double conv_value) { AddCommand(command_creator_.SetWarningThreshold3(conv_value)); }

void RadiationMonitor::ReadDate() { AddCommand(command_creator_.ReadDate()); }
void RadiationMonitor::SetDate(QDateTime timestamp)  { AddCommand(command_creator_.SetDate(timestamp)); }
void RadiationMonitor::ReadModeParameters() { AddCommand(command_creator_.ReadModeParameters()); }
void RadiationMonitor::ReadNumericParameters() { AddCommand(command_creator_.ReadNumericParameters()); }
void RadiationMonitor::ReadThresholdStatus() { AddCommand(command_creator_.ReadThresholdStatus()); }
void RadiationMonitor::ReadInstantenousMeasurement() { AddCommand(command_creator_.ReadInstantenousMeasurement()); }
void RadiationMonitor::ReadIntegratedMeasurement1() { AddCommand(command_creator_.ReadIntegratedMeasurement1()); }
void RadiationMonitor::ReadIntegratedMeasurement2() { AddCommand(command_creator_.ReadIntegratedMeasurement2()); }
void RadiationMonitor::ReadBufferIntegratedMeasurement1() { ForceAddCommand(command_creator_.ReadBufferIntegratedMeasurement1()); }
void RadiationMonitor::ReadBufferIntegratedMeasurement2() { AddCommand(command_creator_.ReadBufferIntegratedMeasurement2()); }

void RadiationMonitor::HandleDetectedXRay(InstantenousMeasurement peak_value) {
    double integrated_charge(0.0);
    try {
        QDateTime fromTime = peak_value.timestamp().addSecs(-2);
        QDateTime toTime = peak_value.timestamp();
        if (ring_buffer_.IntegratedChargeExist(toTime.addSecs(1))) {
            toTime = toTime.addSecs(1);
        }
        integrated_charge = ring_buffer_.GetIntegratedCharge(fromTime, toTime);
    }
    catch (std::exception& exc) {
        qWarning() << "RadiationMonitor::HandleDetectedXRay Could not get integrated charge: " << exc.what();
    }
    qDebug() << "XRay: " << peak_value.timestamp().toString() << " "
             << peak_value.conversed_value() << " " << integrated_charge;
    emit SIGNAL_XRayDetected(peak_value.conversed_value(), integrated_charge);
}

void RadiationMonitor::AddIntegratedToRingBuffer(IntegratedMeasurement m) {
    try {
        ring_buffer_.AddIntegratedValue(m);
    }
    catch (std::exception& exc) {
        qDebug() << "RadiationMonitor::AddIntegratedToRingBuffer Exception: " << exc.what();
        WarningMessage(QString(exc.what()));
    }
}

void RadiationMonitor::AddBufferedToRingBuffer(IntegratedMeasurementBuffer b) {
    try {
        ring_buffer_.AddBufferIntegratedValue(b);
    }
    catch (std::exception& exc) {
        qDebug() << "RadiationMonitor::AddBufferedToRingBuffer Exception: " << exc.what();
        WarningMessage(QString(exc.what()));
    }
}

}
