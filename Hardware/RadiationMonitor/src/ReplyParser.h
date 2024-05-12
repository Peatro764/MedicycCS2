#ifndef REPLYPARSER_H
#define REPLYPARSER_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "Measurement.h"
#include "Definitions.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class ReplyParser : public QObject
{
    Q_OBJECT
public:
    ReplyParser();
    ReplyParser(QString separator, QString footer);
    ~ReplyParser() {}

public slots:
    void Parse(const QByteArray& line);
    void Parse(QString line);
    void SetSeparator(QString separator) { separator_ = separator; }
    void SetFooter(QString footer) { footer_ = footer; }

private slots:

signals:
    void ReplyError(QString msg);
    void BaliseError(QString msg);
    void CommandConfirmed();
    void CommandReply();

    // measurement replies
    void InstantenousMeasurementChanged(InstantenousMeasurement m);
    void IntegratedMeasurement1Changed(IntegratedMeasurement m);
    void IntegratedMeasurement2Changed(IntegratedMeasurement m);
    void BufferIntegratedMeasurement1Changed(IntegratedMeasurementBuffer b);
    void BufferIntegratedMeasurement2Changed(IntegratedMeasurementBuffer b);
    void ThresholdStatusChanged(ThresholdStatus status);

    // Mode parameter replies
    void ModeFunctionnementChanged(ModeFunctionnement mode);
    void PreAmpliTypeChanged(PreAmpliType type);
    void PreAmpliGammeChanged(PreAmpliGamme gamme);
    void EmitInstantenousMeasurementChanged(bool yes);
    void EmitIntegrated1MeasurementChanged(bool yes);
    void EmitIntegrated2MeasurementChanged(bool yes);
    void IncludeDateInMeasurementChanged(bool yes);
    void IncludeConversedValueInMeasurementChanged(bool yes);
    void DateChanged(QDateTime date);

    // Numeric parameter replies
    void InstantenousMeasurementTimeChanged(int seconds);
    void InstantenousMeasurementElementsChanged(int elements);
    void NmbMeasurementsBelowThresholdBeforeAlarmResetChanged(int measurements); // n
    void IntegratedMeasurement1TimeChanged(int minutes);
    void IntegratedMeasurement2TimeChanged(int hours);
    void NmbMeasurementsIntegrationTime2Changed(int measurements); // n
    void InstantenousMeasurementConversionCoeffChanged(double coeff);
    void IntegratedMeasurementConversionCoeffChanged(double coeff);
    void IntegratedMeasurementThresholdLevelChanged(double ampere);
    void WarningThreshold1Changed(double ampere); // n
    void WarningThreshold2Changed(double ampere); // n
    void WarningThreshold3Changed(double ampere); // n

private:
    void ProcessInstantenousMeasurementReply(QStringList body);
    void ProcessIntegratedMeasurement1Reply(QStringList body);
    void ProcessIntegratedMeasurement2Reply(QStringList body);
    void ProcessBufferIntegratedMeasurement1(QStringList body);
    void ProcessBufferIntegratedMeasurement2(QStringList body);

    void ProcessModeParameterReply(QStringList body);
    void ProcessNumericParameterReply(QStringList body);
    void ProcessDate(QStringList body);
    void ProcessErrorReply(QString body);
    void ProcessThresholdStatusReply(QStringList body);
    Measurement ParseMeasurement(QStringList body);
    QDateTime DateFromString(const QStringList& body) const;

    QString footer_ = "\r\n";
    QString separator_ = ":";
};

}

#endif
