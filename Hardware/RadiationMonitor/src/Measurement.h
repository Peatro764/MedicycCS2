#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <QObject>
#include <QString>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class Measurement
{
public:
    Measurement() {}
    Measurement(QDateTime timestamp, double value1, double value2);
    QDateTime timestamp() const { return timestamp_; }
    double value1() const { return value1_; }
    double value2() const { return value2_; }

protected:
    QDateTime timestamp_;
    double value1_ = 0.0;
    double value2_ = 0.0;
};

bool operator==(const Measurement &m1, const Measurement& m2);
bool operator!=(const Measurement &m1, const Measurement& m2);
bool operator<(const Measurement &m1, const Measurement& m2);

class IntegratedMeasurement : public Measurement
{
public:
    IntegratedMeasurement() {}
    IntegratedMeasurement(QDateTime timestamp, double charge, double conversed_value);
    IntegratedMeasurement(const Measurement& m);
    void SetTimeStamp(QDateTime timestamp) { timestamp_ = timestamp; }
    QDateTime timestamp() const { return timestamp_; }
    double charge() const { return value1_; }
    double conversed_value() const { return value2_; }
};

bool operator==(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2);
bool operator!=(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2);
bool operator<(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2);

class InstantenousMeasurement : public Measurement
{
public:
    InstantenousMeasurement() {}
    InstantenousMeasurement(QDateTime timestamp, double current, double conversed_value);
    InstantenousMeasurement(const Measurement& m);
    QDateTime timestamp() const { return timestamp_; }
    double current() const { return value1_; }
    double conversed_value() const { return value2_; }
};

bool operator==(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2);
bool operator!=(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2);
bool operator<(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2);

class IntegratedMeasurementBuffer
{
public:
    IntegratedMeasurementBuffer() {}
    IntegratedMeasurementBuffer(const IntegratedMeasurement& m_pre,
                                const IntegratedMeasurement& m_act);
    IntegratedMeasurement PreviousMeasurement() const { return m_pre_; }
    IntegratedMeasurement ActualMeasurement() const { return m_act_; }
private:
    IntegratedMeasurement m_pre_;
    IntegratedMeasurement m_act_;
};

bool operator==(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2);
bool operator!=(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2);
bool operator<(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2);

}

#endif
