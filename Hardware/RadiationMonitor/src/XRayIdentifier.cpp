#include "XRayIdentifier.h"

#include <stdexcept>
#include <set>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

XRayIdentifier::XRayIdentifier(bool enabled, int min_in_interval, int max_in_interval,
                               double lower_interval, double upper_interval)
    : enabled_(enabled), min_in_interval_(min_in_interval), max_in_interval_(max_in_interval),
      lower_interval_(lower_interval), upper_interval_(upper_interval) {
    qDebug() << "XRayIdentifier nentries" << min_in_interval_ << " -> " << max_in_interval_
             << " thresholds " << lower_interval_ << " -> " << upper_interval_;
}

XRayIdentifier::~XRayIdentifier() {}

void XRayIdentifier::AddInstantaneousValue(InstantenousMeasurement m) {
    if (IsAboveThreshold(m)) {
        in_interval_.clear();
        background_reached_ = false;
    } else if (IsBelowThreshold(m)) {
        CheckForXRaySignature();
        in_interval_.clear();
        background_reached_ = true;
    } else if (IsBetweenThresholds(m) && background_reached_) {
        in_interval_.push_back(m);
    } else {
        // omitted on purpose
    }
}

void XRayIdentifier::CheckForXRaySignature() {
    if (!enabled_) return;
    if (static_cast<int>(in_interval_.size()) >= min_in_interval_ && static_cast<int>(in_interval_.size()) <= max_in_interval_) {
        auto peak_element = std::max_element( in_interval_.begin(), in_interval_.end(),
                                     []( const InstantenousMeasurement &a, const InstantenousMeasurement &b )
                                     {
                                         return a.conversed_value() < b.conversed_value();
                                     } );
        qDebug() << "XRayIdentifier::CheckForXRaySignature X-ray detected";
        emit XRayIdentified(*peak_element);
    }
}

bool XRayIdentifier::IsAboveThreshold(InstantenousMeasurement m) const {
    return (m.conversed_value() > upper_interval_);
}

bool XRayIdentifier::IsBelowThreshold(InstantenousMeasurement m) const {
    return (m.conversed_value() < lower_interval_);
}

bool XRayIdentifier::IsBetweenThresholds(InstantenousMeasurement m) const {
    return (m.conversed_value() >= lower_interval_ && m.conversed_value() <= upper_interval_);
}

}
