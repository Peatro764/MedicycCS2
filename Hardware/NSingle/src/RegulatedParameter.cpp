#include "RegulatedParameter.h"

#include <math.h>
#include <numeric>
#include <cassert>
#include <QDebug>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

RegulatedParameter::RegulatedParameter(NSingleConfig config)
    : config_(config),
      des_value_(config.GetChannel1ValueConfig()),
      set_value_(config.GetChannel1SetPointConfig()),
      buffer_size_(config_.regulation_buffer_size())
{
    SetDefaultTolerance();
    des_value_.SetData(0, true);
    set_value_.SetData(0, true);
}

RegulatedParameter::~RegulatedParameter() {}

void RegulatedParameter::ActValueReceived(const Measurement& m) {
    if (!act_values_.empty()) {
        if (act_values_.back().sign() != m.sign() && !config_.hot_polarity_change()) {
           act_values_.clear();
        }
    }
    act_values_.push_back(m);
    if (static_cast<int>(act_values_.size()) > buffer_size_) {
        act_values_.erase(act_values_.begin());
    }
    CalculateSignalParameters();
    CheckSignalParameters();
    emit SIGNAL_ActUpdated(m.InterpretedValue(), m.sign());
    emit SIGNAL_DesValue(des_value_.InterpretedValue(), des_value_.sign());
}

void RegulatedParameter::SetPointReceived(const Measurement& m) {
    if (m == set_value_) {
        emit SIGNAL_SetPointsMatch();
    } else {
        emit SIGNAL_SetPointsDontMatch();
    }
}

// Make sure to read actual value before
void RegulatedParameter::SetZeroDesValueWithCurrentPolarity() {
    if (act_values_.empty()) {
        qDebug() << "RegulatedParameter::SetZeroDesValue No act values, aborting silently";
        return;
    }
    auto act_value = act_values_.back();
    des_value_.SetData(0, act_value.sign());
    set_value_.SetData(0, act_value.sign());
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

// Make sure to read actual value before
void RegulatedParameter::SetCurrentPolarityInvertedZeroDesValue() {
    if (act_values_.empty()) {
        qDebug() << "RegulatedParameter::SetPolarityInvertedZeroDesValue No act values, aborting silently";
        return;
    }
    auto act_value = act_values_.back();
    des_value_.SetData(0, act_value.inverted_sign());
    set_value_.SetData(0, act_value.inverted_sign());
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();

}

void RegulatedParameter::SetDesValue(const Measurement& m) {
    des_value_ = m;
    set_value_.SetData(des_value_.InterpretedValue(), des_value_.sign());
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

void RegulatedParameter::IncRampSetPoint() {
    qDebug() << "RegulatedParameter::SetRampSetPoint";
    ramp_params_.delta = std::min(config_.ramp_max_delta(), config_.ramp_power_delta()*ramp_params_.delta);
    ramp_params_.current = ramp_params_.current + ramp_params_.dir*ramp_params_.delta;
}

void RegulatedParameter::SetRampSetPoint() {
    const bool finished = (ramp_params_.start < ramp_params_.stop) ?
                (ramp_params_.current > ramp_params_.stop) :
                (ramp_params_.current < ramp_params_.stop);
    if (!finished) {
        qDebug() << "New ramp set point " << ramp_params_.current;
        set_value_.SetData(ramp_params_.current, des_value_.sign());
    } else {
        qDebug() << "Ramp finished";
        emit SIGNAL_RampFinished();
    }
}

void RegulatedParameter::SetRampParameters() {
    // if polarity different, abort
    // if no act values, abort
    const Measurement act = GetActValue();
    ramp_params_.start = act.InterpretedValue();
    ramp_params_.stop = des_value_.InterpretedValue();
    ramp_params_.dir = (std::abs(des_value_.InterpretedValue()) > std::abs(act.InterpretedValue())) ? 1 : -1;
    ramp_params_.delta = 1.0;
    ramp_params_.current = act.InterpretedValue() + (ramp_params_.dir * ramp_params_.delta);
    qDebug() << "Ramp parameters";
    qDebug() << "Start " << ramp_params_.start;
    qDebug() << "Stop " << ramp_params_.stop;
    qDebug() << "Current " << ramp_params_.current;
    qDebug() << "Dir " << ramp_params_.dir;
    qDebug() << "Delta " << ramp_params_.delta;
}

void RegulatedParameter::SetDesPhysicalValue(double value) {
    if (!des_value_.CanContainData(value) || !set_value_.CanContainData(value)) {
        qDebug() << "RegulatedParameter::SetDesPhysicalValue Out of bounds";
        emit SIGNAL_DesValueOutOfBounds();
        return;
    }
    des_value_.SetData(value, des_value_.sign());
    set_value_.SetData(value, des_value_.sign());
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

void RegulatedParameter::ChangePolarity() {
    const bool inverted_sign = !des_value_.sign();
    des_value_.SetData(des_value_.InterpretedValue(), inverted_sign);
    set_value_.SetData(des_value_.InterpretedValue(), inverted_sign);
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

void RegulatedParameter::SetDesPhysicalValue(double value, bool polarity) {
    if (!des_value_.CanContainData(value) || !set_value_.CanContainData(value)) {
        qDebug() << "RegulatedParameter::SetDesPhysicalValue Out of bounds";
        emit SIGNAL_DesValueOutOfBounds();
        return;
    }
    des_value_.SetData(value, polarity);
    set_value_.SetData(value, polarity);
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

void RegulatedParameter::SetActAsDesValue() {
    if (act_values_.empty()) {
        return;
    }
    // Some uni polairty power supplies (Inj33kV notably) has a negative value at low positive set point values
    // Therefore need to change this here.
    Measurement m = act_values_.back();
    if (config_.GetLowLevelConfig().polarity() == Config::Polarity::Uni) {
        m.SetData(m.InterpretedValue(), true);
    }
    SetDesValue(m);
}

void RegulatedParameter::IncrementSetAndDesValue(int nbits) {
    des_value_.Increment(nbits);
    set_value_.Increment(nbits);
    // Just to rapidly update UIs
    emit SIGNAL_DesValue(des_value_.InterpretedValue(), des_value_.sign());
}

void RegulatedParameter::DecrementSetAndDesValue(int nbits) {
    des_value_.Decrement(nbits);
    set_value_.Decrement(nbits);
    // Just to rapidly update UIs
    emit SIGNAL_DesValue(des_value_.InterpretedValue(), des_value_.sign());
}

void RegulatedParameter::IncrementDesValue(int nbits) {
    des_value_.Increment(nbits);
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}

void RegulatedParameter::DecrementDesValue(int nbits) {
    des_value_.Decrement(nbits);
    emit SIGNAL_DesUpdated(des_value_.InterpretedValue(), des_value_.sign());
    CheckSignalParameters();
}


int RegulatedParameter::DesAndActValueDifference() {
    if (act_values_.empty()) {
        return des_value_.RawValue();
    } else {
        return (des_value_.RawValue() - act_values_.back().RawValue());
    }
}

void RegulatedParameter::SetDefaultTolerance() {
    mean_tol_ = static_cast<double>(config_.GetChannel1ValueConfig().min() / 1.99);
    std_tol_ = static_cast<double>(config_.signal_variation() / 2.0);
}

void RegulatedParameter::SetDoubleTolerance() {
    mean_tol_ = 2.0 * mean_tol_;
    std_tol_ = 2.0 * std_tol_;
}

void RegulatedParameter::SetReadyTolerance() {
    mean_tol_ = static_cast<double>(config_.signal_variation()) / (2.0 * std::sqrt(2.0));
    std_tol_ = static_cast<double>(config_.signal_variation()) / 2.0;
}

Measurement RegulatedParameter::GetActValue() const {
    if (!act_values_.empty()) {
        return act_values_.back();
    } else {
        Measurement m = des_value_;
        m.SetData(0, m.sign());
        return m;
    }
}

Measurement RegulatedParameter::GetDesValue() const {
    return des_value_;
}

bool RegulatedParameter::GetActPolarity() const {
    Measurement m = GetActValue();
    return m.sign();
}

void RegulatedParameter::SetImprovedSetPoint() {
    if (act_values_.empty()) {
        return;
    }
    const double offset_value_bits = (mean_ - static_cast<double>(des_value_.RawValue()));
    const double offset_physical = offset_value_bits * config_.GetChannel1ValueConfig().bit_value();
    const double offset_set_bits = offset_physical / (config_.GetChannel1SetPointConfig().bit_value() * config_.GetChannel1SetPointConfig().min());

    // step magnitude
    int steps(1);
    if (std::abs(offset_set_bits) > 10.0) {
        steps = 5;
    } else if (std::abs(offset_set_bits) > 5.0) {
        steps = 3;
    } else {
        steps = 1;
    }

    // step direction
    if (offset_set_bits > 0.5) {
        set_value_.Decrement(steps);
    } else if (offset_set_bits < -0.5) {
        set_value_.Increment(steps);
    } // else do not change set point since it differs by less than half a bit in any direction
}

void RegulatedParameter::SetPointToDesPoint() {
    set_value_.SetData(des_value_.InterpretedValue(), des_value_.sign());
}

Measurement RegulatedParameter::GetSetPoint() const {
    return set_value_;
}

void RegulatedParameter::CalculateSignalParameters() {
    if (act_values_.empty()) {
        mean_ = 0.0;
        stddev_ = 0.0;
    } else {
        int sum = 0.0;
        for (auto& m : act_values_) {
            sum += m.RawValue();
        }
        mean_ = static_cast<double>(sum) / static_cast<double>(act_values_.size());
        if (act_values_.size() < 2) {
            stddev_ = 0.0;
            stderr_ = 0.0;
        } else {
           std::vector<double> diff(act_values_.size());
           std::transform(act_values_.begin(), act_values_.end(), diff.begin(), [this](const Measurement& m) { return static_cast<double>(m.RawValue()) - mean_; });
           double sqrSum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
           stddev_ = std::sqrt(sqrSum / act_values_.size());
           stderr_ = stddev_ / std::sqrt(act_values_.size());
        }
    }
}

void RegulatedParameter::CheckSignalParameters() {
    //if (!act_values_.empty()) qDebug() <<  config_.name() <<  " des_value " << des_value_.HexValue() << " set_value " << set_value_.HexValue() << " act_value " << act_values_.back().HexValue();
    if (SignalSteady()) {
        //qDebug() << "SignalSteady";
        emit SIGNAL_SignalSteady();
    } else {
        //qDebug() << "SignalChange";
        emit SIGNAL_SignalChange();
    }
    if (NearTarget()) {
        //qDebug() << "NearTarget";
        emit SIGNAL_NearTarget();
    }
    if (NearZero()) {
        //qDebug() << "NearZero";
        emit SIGNAL_NearZero();
    }
    if (OnTarget()) {
        //qDebug() << config_.name() << " OnTarget";
        emit SIGNAL_OnTarget();
    } else {
        //qDebug() << config_.name() << " OffTarget";
        emit SIGNAL_OffTarget();
    }
    if (PolarityCorrect()) {
        emit SIGNAL_PolarityCorrect();
    } else {
        emit SIGNAL_PolarityWrong();
    }
}

bool RegulatedParameter::SignalSteady() const {
    if (act_values_.size() < 2) {
        return false;
    }
    //qDebug() << "SignaSteady stddev " << stddev_ << " tol " << std_tol_;
    return (stddev_ < std_tol_);
}

bool RegulatedParameter::NearTarget() const {
    if (act_values_.empty()) {
        return false;
    }
    //qDebug() << "NearTarget: des - act " << std::abs(des_value_.RawValue() - act_values_.back().RawValue()) << " tol " << config_.near_target_limit();
    return (std::abs(des_value_.RawValue() - act_values_.back().RawValue()) < config_.near_target_limit());
}

bool RegulatedParameter::NearZero() const {
    if (act_values_.empty()) {
        return false;
    }
    //qDebug() << "NearZero: raw " << act_values_.back().RawValue() << " tol " << config_.near_zero_limit();
    return (act_values_.back().RawValue() < config_.near_zero_limit());
}

bool RegulatedParameter::OnTarget() const {
    if (act_values_.empty()) {
        return false;
    }
    //qDebug() << config_.name() <<  "OnTarget: Mean " << mean_ << " Des " << des_value_.RawValue() << " tol " << mean_tol_;
    const bool mean_ok = (std::abs(static_cast<double>(des_value_.RawValue()) - mean_) < mean_tol_);
    if (!mean_ok) {
        return false;
    }
    const bool polarity_ok = (des_value_.sign() == act_values_.back().sign());
    if (!config_.hot_polarity_change() && !polarity_ok) {
        return false;
    }
    const bool signal_steady = SignalSteady();
    return signal_steady;
}

bool RegulatedParameter::PolarityCorrect() const {
    if (act_values_.empty()) {
        return true;
    } else {
        return (act_values_.back().sign() == des_value_.sign());
    }
}

} // ns
