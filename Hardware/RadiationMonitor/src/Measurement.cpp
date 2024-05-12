#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

Measurement::Measurement(QDateTime timestamp, double value1, double value2)
    : timestamp_(timestamp), value1_(value1), value2_(value2) {}

bool operator==(const Measurement &m1, const Measurement& m2) {
    return m1.timestamp() == m2.timestamp();
}

bool operator!=(const Measurement &m1, const Measurement& m2) {
    return !(m1 == m2);
}

bool operator<(const Measurement &m1, const Measurement& m2) {
    return m1.timestamp() < m2.timestamp();
}

IntegratedMeasurement::IntegratedMeasurement(QDateTime timestamp, double charge, double conversed_value)
    : Measurement(timestamp, charge, conversed_value) {}

IntegratedMeasurement::IntegratedMeasurement(const Measurement &m)
    : Measurement(m) {}

bool operator==(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2) {
    return m1.timestamp() == m2.timestamp();
}

bool operator!=(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2) {
    return !(m1 == m2);
}

bool operator<(const IntegratedMeasurement &m1, const IntegratedMeasurement& m2) {
    return m1.timestamp() < m2.timestamp();
}


InstantenousMeasurement::InstantenousMeasurement(QDateTime timestamp, double current, double conversed_value)
    : Measurement(timestamp, current, conversed_value) {}

InstantenousMeasurement::InstantenousMeasurement(const Measurement& m)
    : Measurement(m) {}

bool operator==(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2) {
    return m1.timestamp() == m2.timestamp();
}

bool operator!=(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2) {
    return !(m1 == m2);
}

bool operator<(const InstantenousMeasurement &m1, const InstantenousMeasurement& m2) {
    return m1.timestamp() < m2.timestamp();
}

IntegratedMeasurementBuffer::IntegratedMeasurementBuffer(const IntegratedMeasurement &m_pre,
                                                         const IntegratedMeasurement &m_act)
    : m_pre_(m_pre), m_act_(m_act) {}

bool operator==(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2) {
    return m1.ActualMeasurement() == m2.ActualMeasurement();
}

bool operator!=(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2) {
    return !(m1.ActualMeasurement() == m2.ActualMeasurement());
}

bool operator<(const IntegratedMeasurementBuffer &m1, const IntegratedMeasurementBuffer& m2) {
    return m1.ActualMeasurement() < m2.ActualMeasurement();
}

}
