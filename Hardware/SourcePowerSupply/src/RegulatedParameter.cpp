#include "RegulatedParameter.h"

#include <numeric>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

RegulatedParameter::RegulatedParameter(RegulatedParameterConfig config)
    : def_tolerance_(config.tolerance()), act_tolerance_(config.tolerance()), near_zero_limit_(config.near_zero()),
      near_target_limit_(config.near_target()), lower_limit_(config.lower_limit()), upper_limit_(config.upper_limit()), max_entries_(config.buffer_size()), debug_(false) {
    if (max_entries_ < 1) {
       throw std::runtime_error("RegulatedParameter max_entires must be larger than 1");
    }
}

RegulatedParameter::RegulatedParameter(double tolerance, double near_zero, double near_target, double lower_limit, double upper_limit, int max_entries, bool debug)
    : def_tolerance_(tolerance), act_tolerance_(tolerance), near_zero_limit_(near_zero), near_target_limit_(near_target), lower_limit_(lower_limit), upper_limit_(upper_limit), max_entries_(max_entries), debug_(debug) {
    if (max_entries < 1) {
       throw std::runtime_error("RegulatedParameter max_entires must be larger than 1");
    }
}

RegulatedParameter::~RegulatedParameter() {}

void RegulatedParameter::AddActValue(double value) {
    act_values_.push_back(value);
    if (static_cast<int>(act_values_.size()) > max_entries_) {
        act_values_.erase(act_values_.begin());
    }
    CalculateSignalParameters();
    EvaluateThresholds();
}

void RegulatedParameter::SetDesValue(double value) {
    des_value_ = value;
    emit SIGNAL_DesValueUpdated(des_value_);
    EvaluateThresholds();
}

void RegulatedParameter::SetTolerance(double value) {
    def_tolerance_ = value;
    act_tolerance_ = value;
    EvaluateThresholds();
}

void RegulatedParameter::DoubleTolerance() {
    qDebug() << "RegulatedParameter::DoubleTolerance";
    act_tolerance_ = 2.0 * act_tolerance_;
    EvaluateThresholds();
}

void RegulatedParameter::SetDefaultTolerance() {
    qDebug() << "RegulatedParameter::SetDefaultTolerance";
    act_tolerance_ = def_tolerance_;
    EvaluateThresholds();
}

void RegulatedParameter::SetLimits(double lower, double upper) {
    lower_limit_ = lower;
    upper_limit_ = upper;
}

void RegulatedParameter::SetActAsDesValue() {
    if (!act_values_.empty()) {
        qDebug() << "RegulatedParameter::UseCurrentAsSetPoint Update from " << des_value_ << " to " << act_values_.back();
        des_value_ = act_values_.back();
        emit SIGNAL_DesValueUpdated(des_value_);
        EvaluateThresholds();
    } else {
        qWarning() << "RegulatedParameter::UseCurrentAsSetPoint No entries in vector";
    }
}

SETPOINTCORR RegulatedParameter::GetSetPointCorrection() const {
    if (act_values_.empty()) {
        return SETPOINTCORR::STA;
    } else if (std::abs(act_values_.back() - des_value_) < act_tolerance_) {
        return SETPOINTCORR::STA;
    } else if (act_values_.back() > des_value_) {
        return SETPOINTCORR::DEC;
    } else {
        return SETPOINTCORR::INC;
    }
}

SETPOINTCORR RegulatedParameter::GetMeanSetPointCorrection() const {
    if (act_values_.empty()) {
        return SETPOINTCORR::STA;
    } else if (std::abs(mean_ - des_value_) < act_tolerance_) {
        return SETPOINTCORR::STA;
    } else if (mean_ > des_value_) {
        return SETPOINTCORR::DEC;
    } else {
        return SETPOINTCORR::INC;
    }
}

bool RegulatedParameter::AllowedSetPoint(double value) const {
    if(act_values_.empty()) {
        return (value >= lower_limit_ && value <= upper_limit_);
    } else {
        const double act_value = act_values_.back();
        if (act_value > value) {
            return !LowerLimitPassed();
        } else {
            return !UpperLimitPassed();
        }
    }
}

bool RegulatedParameter::UpperLimitPassed() const {
    if (!act_values_.empty()) {
        const double act_value = act_values_.back();
        return (act_value > upper_limit_);
    }  else {
        return false;
    }
}

bool RegulatedParameter::LowerLimitPassed() const {
    if (!act_values_.empty()) {
        const double act_value = act_values_.back();
        return (act_value < lower_limit_);
    }  else {
        return false;
    }
}

void RegulatedParameter::EvaluateThresholds() {
    if (NearZero()) {
        emit SIGNAL_Zero();
    } else {
        emit SIGNAL_NotZero();
    }
    if (OnTarget()) {
        emit SIGNAL_OnTarget();
    } else {
        emit SIGNAL_OffTarget();
    }
    if (NearTarget()) {
        emit SIGNAL_NearTarget();
    } else {
        emit SIGNAL_FarFromTarget();
    }
    if (Stable()) {
        emit SIGNAL_Stable();
    } else {
        emit SIGNAL_UnStable();
    }
}

bool RegulatedParameter::NearZero() const {
    if (act_values_.empty()) {
        return false;
    } else {
        const bool near_zero = (std::abs(act_values_.back()) < near_zero_limit_);
        qDebug() << "NearZero " << near_zero;
        return near_zero;
    }
}

bool RegulatedParameter::OnTarget() const {
    if (act_values_.empty()) {
        return false;
    } else {
        const bool mean_ok = std::abs(mean_ - des_value_) < act_tolerance_;
        const bool stddev_ok = (stddev_ < act_tolerance_ * 2);
        qDebug() << "ONTARGET Act " << act_values_.back() <<  " Mean " << mean_ << " Des " << des_value_ <<  " StdDev " << stddev_ << " tol " << act_tolerance_ << " MEAN " << mean_ok << " STDOK " << stddev_ok;
        return (mean_ok && stddev_ok);
    }
}

bool RegulatedParameter::NearTarget() const {
    if (act_values_.empty()) {
        return false;
    } else {
        const bool near_target = std::abs(act_values_.back() - des_value_) < near_target_limit_;
        qDebug() << "NEARTARGET Act " << act_values_.back() << " Des " << des_value_ << " Diff " << std::abs(act_values_.back() - des_value_) << " MaxDiff " << near_target_limit_ << " NearTarget " << near_target;
        return near_target;
    }
}

bool RegulatedParameter::Stable() const {
    if (act_values_.size() < 2) {
        return false;
    }

    const double sum = std::accumulate(act_values_.begin(), act_values_.end(), 0.0);
    const double mean = sum / static_cast<double>(act_values_.size());
    std::vector<double> diff(act_values_.size());
    std::transform(act_values_.begin(), act_values_.end(), diff.begin(), [mean](double m) { return (m - mean); });
    const double sqrSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
    const double stddev = std::sqrt(sqrSum / act_values_.size());
    //const double stderr = stddev / std::sqrt(act_values_.size());

    return (stddev < (act_tolerance_ / 2.0));
}

void RegulatedParameter::CalculateSignalParameters() {
    if (act_values_.empty()) {
        mean_ = 0.0;
        stddev_ = 0.0;
    } else {
        mean_ = std::accumulate(act_values_.begin(), act_values_.end(), 0.0) / static_cast<double>(act_values_.size());
        if (act_values_.size() < 2) {
            stddev_ = 0.0;
        } else {
           std::vector<double> diff(act_values_.size());
           std::transform(act_values_.begin(), act_values_.end(), diff.begin(), [this](double m) { return (m - mean_); });
           double sqrSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
           stddev_ = std::sqrt(sqrSum / act_values_.size());
        }
    }
}

}
