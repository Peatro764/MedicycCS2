#ifndef REGULATEDPARAMETERCONFIG_H
#define REGULATEDPARAMETERCONFIG_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

class RegulatedParameterConfig {
public:
    RegulatedParameterConfig();
    RegulatedParameterConfig(double tolerance,
                             double near_zero,
                             double near_target,
                             double lower_limit,
                             double upper_limit,
                             int buffer_size);
    double tolerance() const { return tolerance_; }
    double near_zero() const { return near_zero_; }
    double near_target() const { return near_target_; }
    double lower_limit() const { return lower_limit_; }
    double upper_limit() const { return upper_limit_; }
    int buffer_size() const { return buffer_size_; }
private:
    friend bool operator==(const RegulatedParameterConfig &c1, const RegulatedParameterConfig& c2);
    double tolerance_;
    double near_zero_;
    double near_target_;
    double lower_limit_;
    double upper_limit_;
    int buffer_size_;
};

bool operator==(const RegulatedParameterConfig &c1, const RegulatedParameterConfig& c2);
bool operator!=(const RegulatedParameterConfig &c1, const RegulatedParameterConfig& c2);

}

#endif
