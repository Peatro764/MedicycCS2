#ifndef COMMANDCREATOR_H
#define COMMANDCREATOR_H

#include <QString>
#include <QDebug>

#include "Definitions.h"
#include "Command.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class CommandCreator
{
public:
    CommandCreator();
    CommandCreator(QString footer);
    ~CommandCreator();

    void SetFooter(QString footer) { footer_ = footer; }

    Command PowerOn() const;
    Command StartMeasurement() const;
    Command StopMeasurement(bool wait_confirmation) const;
    Command SetSeparator(QString separator) const;
    Command EnableRemote(bool wait_confirmation) const;
    Command EnableLocal() const;
    Command EnableReply() const;
    Command DisableReply() const;

    Command SetModeFunctionnement(ModeFunctionnement mode) const;
    Command SetPreAmpliType(PreAmpliType type) const;
    Command SetPreAmpliGamme(PreAmpliGamme gamme) const;
    Command SetEmitInstanenousMeasurement(bool yes) const;
    Command SetEmitIntegrated1Measurement(bool yes) const;
    Command SetEmitIntegrated2Measurement(bool yes) const;
    Command SetIncludeDateInReply(bool yes) const;
    Command SetIncludeRawMeasurementInReply(bool yes) const;

    Command SetInstantenousMeasurementTime(int seconds) const;
    Command SetInstantenousMeasurementElements(int elements) const;
    Command SetNmbMeasurementsBelowThresholdBeforeAlarmReset(int measurements) const;
    Command SetIntegratedMeasurement1Time(int minutes) const;
    Command SetIntegratedMeasurement2Time(int hours) const;
    Command SetNmbMeasurementsIntegrationTime2(int measurements) const;
    Command SetInstantenousMeasurementConversionCoefficient(double coeff) const;
    Command SetIntegratedMeasurementConversionCoefficient(double coeff) const;
    Command SetIntegratedMeasurementThresholdLevel(double ampere) const;
    Command SetWarningThreshold1(double ampere) const;
    Command SetWarningThreshold2(double ampere) const;
    Command SetWarningThreshold3(double ampere) const;

    Command ReadDate() const;
    Command SetDate(QDateTime timestamp) const;
    Command ReadModeParameters() const; // LP0
    Command ReadNumericParameters() const; // LP1
    Command ReadThresholdStatus() const; // LST
    Command ReadInstantenousMeasurement() const; // L0I
    Command ReadIntegratedMeasurement1() const; // L1I
    Command ReadIntegratedMeasurement2() const; // L2I
    Command ReadBufferIntegratedMeasurement1() const; // BU1
    Command ReadBufferIntegratedMeasurement2() const; // BU2

private:
    QString CreateModeCommand(ModeCommand cmd, int value) const;
    QString CreateParameterCommand(ParameterCommand cmd, QString value) const;
    QString footer_ = "\r\n";

};

}

#endif
