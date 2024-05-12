#include "MeasurementRingBuffer.h"

#include <stdexcept>
#include <set>
#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

MeasurementRingBuffer::MeasurementRingBuffer(int size) : size_(size) {
    size_ = std::max(1, size_);
}

MeasurementRingBuffer::~MeasurementRingBuffer() {}

void MeasurementRingBuffer::AddIntegratedValue(IntegratedMeasurement m) {
    m.SetTimeStamp(QDateTime::currentDateTime());
    qDebug() << "IntegratedValue " << m.timestamp().toString() << " " << m.charge();
    if (!values_ring_.empty()) {
        if (m.timestamp() == values_ring_.back().timestamp()) {
            qDebug() << "MeasurementRingBuffer::AddIntegratedValue timestamp identical, not adding";
            return;
        }

        if ((m.timestamp() > values_ring_.back().timestamp().addSecs(57) && m.timestamp() < values_ring_.back().timestamp().addSecs(63)) ||
            m.timestamp() < values_ring_.back().timestamp()) {
            qDebug() << "MeasurementRingBuffer::AddIntegratedValue Timestamp of new value corrupt. New " << m.timestamp().toString()
                     << " Old " << values_ring_.back().timestamp().toString();
            throw std::runtime_error("Timestamp of integrated measurement corrupt");
        }

        if (m.charge() < values_ring_.back().charge()) emit IntegratedChargeDecreased();
    }
    values_ring_.push_back(m);
    if (static_cast<int>(values_ring_.size()) > size_) {
        values_ring_.erase(values_ring_.begin());
    }
}

void MeasurementRingBuffer::AddBufferIntegratedValue(IntegratedMeasurementBuffer m) {
    qDebug() << "BufferIntegratedValue " << m.ActualMeasurement().timestamp().toString() << m.ActualMeasurement().charge();
    if (!buffer_ring_.empty()) {
        if (m.ActualMeasurement().timestamp() == buffer_ring_.back().ActualMeasurement().timestamp()){
            qDebug() << "MeasurementRingBuffer::AddBufferIntegratedValue New value has same timestamp as old one, skipping";
            return;
        }
        if (m.ActualMeasurement().timestamp() < buffer_ring_.back().ActualMeasurement().timestamp())
            throw std::runtime_error("New integrated buffer value has a timestamp older than the previous one");
    }
    buffer_ring_.push_back(m);
    if (static_cast<int>(buffer_ring_.size()) > size_) {
        buffer_ring_.erase(buffer_ring_.begin());
    }
}

void MeasurementRingBuffer::ClearBuffer() {
    values_ring_.clear();
    buffer_ring_.clear();
}

void MeasurementRingBuffer::DumpBuffer() const {
    // TODO
}

bool MeasurementRingBuffer::IntegratedChargeExist(QDateTime timestamp) const {
    try {
        GetClosestIntegratedValue(timestamp);
        return true;
    }
    catch (std::exception& exc) {
        return false;
    }
}

double MeasurementRingBuffer::GetIntegratedCharge(QDateTime t_start, QDateTime t_stop) const {
    IntegratedMeasurement start_integrated_value = GetClosestIntegratedValue(t_start);
    IntegratedMeasurement stop_integrated_value = GetClosestIntegratedValue(t_stop);
    double buffer_contribution = GetBufferContributionInInterval(start_integrated_value.timestamp(),
                                                                 stop_integrated_value.timestamp());
    return (stop_integrated_value.charge() - start_integrated_value.charge() + buffer_contribution);
}

IntegratedMeasurement MeasurementRingBuffer::GetClosestIntegratedValue(QDateTime t) const {
    auto InInterval = [&] (const IntegratedMeasurement& m1, const IntegratedMeasurement& m2) -> bool { return (t >= m1.timestamp() && t <= m2.timestamp()); };
    auto it = std::adjacent_find(values_ring_.begin(), values_ring_.end(), InInterval);
    if (it == values_ring_.end()) {
        throw std::runtime_error("There is no measurement at the given time: " + t.toString().toStdString());
    }
    const IntegratedMeasurement m1 = *it;
    const IntegratedMeasurement m2 = *(++it);
    if (std::fabs(m1.timestamp().toMSecsSinceEpoch() - t.toMSecsSinceEpoch()) <=
            std::fabs(m2.timestamp().toMSecsSinceEpoch() - t.toMSecsSinceEpoch())) {
        return m1;
    } else {
        return m2;
    }
}

double MeasurementRingBuffer::GetBufferContributionInInterval(QDateTime t_start, QDateTime t_stop) const {
    std::vector<IntegratedMeasurementBuffer> elements_in_interval;
    std::copy_if(buffer_ring_.begin(), buffer_ring_.end(), std::back_inserter(elements_in_interval),
                 [&](IntegratedMeasurementBuffer m) { return m.ActualMeasurement().timestamp() > t_start &&
                                                             m.ActualMeasurement().timestamp() <= t_stop; });
    std::set<IntegratedMeasurementBuffer> unique_elements(elements_in_interval.begin(), elements_in_interval.end());
    double sum_charge(0.0);
    for (auto& m : unique_elements) {
        sum_charge += m.ActualMeasurement().charge();
    }
    return sum_charge;
}

QDateTime MeasurementRingBuffer::CurrentTime() const {
    if (values_ring_.empty()) {
        throw std::runtime_error("No measurements in buffer");
    }
    return values_ring_.back().timestamp();
}

}
