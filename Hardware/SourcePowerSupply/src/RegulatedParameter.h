#ifndef REGULATEDPARAMETER_H
#define REGULATEDPARAMETER_H

#include <QObject>
#include <QString>
#include <QDebug>

#include "RegulatedParameterConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

enum class SETPOINTCORR { DEC, STA, INC };

class RegulatedParameter : public QObject {
    Q_OBJECT

public:
    RegulatedParameter(RegulatedParameterConfig config);
    RegulatedParameter(double tolerance, double near_zero, double near_target, double lower_limit,
                       double upper_limit, int max_entries, bool debug);
    ~RegulatedParameter();

    void AddActValue(double value);
    void SetDesValue(double value);
    void SetTolerance(double value);
    void DoubleTolerance();
    void SetDefaultTolerance();
    void SetLimits(double lower, double upper);
    void SetActAsDesValue();
    SETPOINTCORR GetSetPointCorrection() const;
    SETPOINTCORR GetMeanSetPointCorrection() const;
    bool LowerLimitPassed() const;
    bool UpperLimitPassed() const;

    // Limits
    bool AllowedSetPoint(double value) const;

    // Made public for unit testing purposes
    bool NearZero() const;
    bool OnTarget() const;
    bool NearTarget() const;
    bool Stable() const;

signals:
    void SIGNAL_Zero();
    void SIGNAL_NotZero();
    void SIGNAL_OnTarget();
    void SIGNAL_OffTarget();
    void SIGNAL_NearTarget();
    void SIGNAL_FarFromTarget();
    void SIGNAL_Stable();
    void SIGNAL_UnStable();
    void SIGNAL_DesValueUpdated(double value);

private:
    void CalculateSignalParameters();
    void EvaluateThresholds();
    double des_value_ = 0.0;
    std::vector<double> act_values_;
    double def_tolerance_ = 0.0;
    double act_tolerance_ = 0.0;
    double near_zero_limit_ = 0.0;
    double near_target_limit_ = 0.0;
    double lower_limit_ = 0;
    double upper_limit_ = 0;
    int max_entries_ = 1;
    double mean_ = 0.0;
    double stddev_ = 0.0;
    bool debug_ = false;
};

}

#endif
