#ifndef MEASUREMENTRINGBUFFER_H
#define MEASUREMENTRINGBUFFER_H

#include <QString>
#include <QDateTime>
#include <vector>

#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class MeasurementRingBuffer : public QObject {
    Q_OBJECT
public:
    MeasurementRingBuffer(int size);
    ~MeasurementRingBuffer();
    bool IntegratedChargeExist(QDateTime timestamp) const;
    void AddIntegratedValue(IntegratedMeasurement m);
    void AddBufferIntegratedValue(IntegratedMeasurementBuffer m);
    double GetIntegratedCharge(QDateTime t_start, QDateTime t_stop) const;
    void ClearBuffer();
    void DumpBuffer() const;
    QDateTime CurrentTime() const;

signals:
    void IntegratedChargeDecreased();

private:
    IntegratedMeasurement GetClosestIntegratedValue(QDateTime t) const;
    double GetBufferContributionInInterval(QDateTime t_start, QDateTime t_stop) const;
    std::vector<IntegratedMeasurement> values_ring_;
    std::vector<IntegratedMeasurementBuffer> buffer_ring_;
    int size_ = 0;
};

}

#endif
