#ifndef XRAYIDENTIFIER_H
#define XRAYIDENTIFIER_H

#include <QString>
#include <QDateTime>
#include <vector>

#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class XRayIdentifier : public QObject {
    Q_OBJECT
public:
    XRayIdentifier(bool enable, int min_in_interval, int max_in_interval,
                   double lower_interval, double upper_interval);
    ~XRayIdentifier();
    bool enabled() { return enabled_; }
    void AddInstantaneousValue(InstantenousMeasurement m);

signals:
    void XRayIdentified(InstantenousMeasurement peak_value);

private:
    void CheckForXRaySignature();
    bool IsBelowThreshold(InstantenousMeasurement m) const;
    bool IsAboveThreshold(InstantenousMeasurement m) const;
    bool IsBetweenThresholds(InstantenousMeasurement m) const;
    std::vector<InstantenousMeasurement> in_interval_;
    bool enabled_ = false;
    bool background_reached_ = false;
    int min_in_interval_ = 1;
    int max_in_interval_ = 3;
    double lower_interval_ = 0.8E-5;
    double upper_interval_ = 8.0E-5;
};

}

#endif
