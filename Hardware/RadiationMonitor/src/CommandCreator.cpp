#include "CommandCreator.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

CommandCreator::CommandCreator() {}

CommandCreator::CommandCreator(QString footer)
    : footer_(footer)
{}

CommandCreator::~CommandCreator() {}

Command CommandCreator::CommandCreator::PowerOn() const {
    return Command(false, false, QString("MST"));
}

Command CommandCreator::CommandCreator::StartMeasurement() const {
    return Command(true, false, QString("MEA 1"));
}

Command CommandCreator::CommandCreator::StopMeasurement(bool wait_confirmation) const {
    return Command(wait_confirmation, false, QString("MEA 0"));
}

Command CommandCreator::CommandCreator::SetSeparator(QString separator) const {
    return Command(true, false, QString("SEP ") + separator);
}

Command CommandCreator::CommandCreator::EnableRemote(bool wait_confirmation) const {
    return Command(wait_confirmation, false, QString("REM"));
}

Command CommandCreator::CommandCreator::EnableLocal() const {
    return Command(false, false, QString("LOC"));
}

Command CommandCreator::CommandCreator::EnableReply() const {
    return Command(true, false, QString("REP 1"));
}

Command CommandCreator::CommandCreator::DisableReply() const {
    return Command(false, false, QString("REP 0"));
}

QString CommandCreator::CreateModeCommand(ModeCommand cmd, int value) const {
    return QString("PR0") + " " + QString::number(static_cast<int>(cmd)) + " " + QString::number(value);
}

QString CommandCreator::CreateParameterCommand(ParameterCommand cmd, QString value) const {
    return QString("PR1") + " " + QString::number(static_cast<int>(cmd)) + " " + value;
}

Command CommandCreator::CommandCreator::SetModeFunctionnement(ModeFunctionnement mode) const {
    QString body = CreateModeCommand(ModeCommand::MODEFUNCTIONNEMENT, static_cast<int>(mode));
    return Command(true, false, body);
}

Command CommandCreator::SetPreAmpliType(PreAmpliType type) const {
    QString body = CreateModeCommand(ModeCommand::PREAMPLITYPE, static_cast<int>(type));
    return Command(true, false, body);
}

Command CommandCreator::SetPreAmpliGamme(PreAmpliGamme gamme) const {
    QString body = CreateModeCommand(ModeCommand::PREAMPLIGAMME, static_cast<int>(gamme));
    return Command(true, false, body);
}

Command CommandCreator::SetEmitInstanenousMeasurement(bool yes) const {
    QString body = CreateModeCommand(ModeCommand::EMITINSTANTENOUSMEASUREMENT, static_cast<int>(yes));
    return Command(true, false, body);
}

Command CommandCreator::SetEmitIntegrated1Measurement(bool yes) const {
    QString body = CreateModeCommand(ModeCommand::EMITINTEGRATED1MEASUREMENT, static_cast<int>(yes));
    return Command(true, false, body);
}

Command CommandCreator::SetEmitIntegrated2Measurement(bool yes) const {
    QString body = CreateModeCommand(ModeCommand::EMITINTEGRATED2MEASUREMENT, static_cast<int>(yes));
    return Command(true, false, body);
}

Command CommandCreator::SetIncludeDateInReply(bool yes) const {
    QString body = CreateModeCommand(ModeCommand::INCLUDEDATEINREPLY, static_cast<int>(yes));
    return Command(true, false, body);
}

Command CommandCreator::SetIncludeRawMeasurementInReply(bool yes) const {
    QString body = CreateModeCommand(ModeCommand::INCLUDERAWMEASUREMENTINREPLY, static_cast<int>(yes));
    return Command(true, false, body);
}

Command CommandCreator::SetInstantenousMeasurementTime(int seconds) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINSTANTENOUSMEASUREMENTTIME, QString::number(seconds));
    return Command(true, false, body);
}

Command CommandCreator::SetInstantenousMeasurementElements(int elements) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINSTANTENOUSMEASUREMENTUNITS, QString::number(elements));
    return Command(true, false, body);
}

Command CommandCreator::SetNmbMeasurementsBelowThresholdBeforeAlarmReset(int measurements) const {
    QString body = CreateParameterCommand(ParameterCommand::NMBMEASUREMENTSBELOWTHRESHOLDBEFOREALARMRESET, QString::number(measurements));
    return Command(true, false, body);
}

Command CommandCreator::SetIntegratedMeasurement1Time(int minutes) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINTEGRATIONTIME1, QString::number(minutes));
    return Command(true, false, body);
}

Command CommandCreator::SetIntegratedMeasurement2Time(int hours) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINTEGRATIONTIME2, QString::number(hours));
    return Command(true, false, body);
}

Command CommandCreator::SetNmbMeasurementsIntegrationTime2(int measurements) const {
    QString body = CreateParameterCommand(ParameterCommand::NMBMEASUREMENTSINTEGRATION2, QString::number(measurements));
    return Command(true, false, body);
}

Command CommandCreator::SetInstantenousMeasurementConversionCoefficient(double coeff) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINSTANTENOUSCONVERSIONCOEFF, QString::number(coeff, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::SetIntegratedMeasurementConversionCoefficient(double coeff) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINTEGRATEDCONVERSIONCOEFF, QString::number(coeff, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::SetIntegratedMeasurementThresholdLevel(double ampere) const {
    QString body = CreateParameterCommand(ParameterCommand::SETINTEGRATIONTHRESHOLD, QString::number(ampere, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::SetWarningThreshold1(double ampere) const {
    QString body = CreateParameterCommand(ParameterCommand::WARNING1THRESHOLD, QString::number(ampere, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::SetWarningThreshold2(double ampere) const {
    QString body = CreateParameterCommand(ParameterCommand::WARNING2THRESHOLD, QString::number(ampere, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::SetWarningThreshold3(double ampere) const {
    QString body = CreateParameterCommand(ParameterCommand::WARNING3THRESHOLD, QString::number(ampere, 'E', 3));
    return Command(true, false, body);
}

Command CommandCreator::ReadDate() const {
    return Command(true, true, QString("LDA"));
}

Command CommandCreator::SetDate(QDateTime timestamp) const {
    timestamp = timestamp.addYears(-100);
    return Command(true, false, QString("PDA ") + timestamp.toString("yy M d h m s"));

}

Command CommandCreator::ReadModeParameters() const {
    return Command(true, true, QString("LP0"));
}

Command CommandCreator::ReadNumericParameters() const {
    return Command(true, true, QString("LP1"));
}

Command CommandCreator::ReadThresholdStatus() const {
    return Command(true, true, QString("LST"));
}

Command CommandCreator::ReadInstantenousMeasurement() const {
    return Command(true, true, QString("L0I"));
}

Command CommandCreator::ReadIntegratedMeasurement1() const {
    return Command(true, true, QString("L1I"));
}

Command CommandCreator::ReadIntegratedMeasurement2() const {
    return Command(true, true, QString("L2I"));
}

Command CommandCreator::ReadBufferIntegratedMeasurement1() const {
    return Command(true, true, QString("BU1"));
}

Command CommandCreator::ReadBufferIntegratedMeasurement2() const {
    return Command(true, true, QString("BU2"));
}

}
