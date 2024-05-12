#include "RegulatedParameterConfig.h"

#include <numeric>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

RegulatedParameterConfig::RegulatedParameterConfig() {

}

RegulatedParameterConfig::RegulatedParameterConfig(double tolerance,
                                                   double near_zero,
                                                   double near_target,
                                                   double lower_limit,
                                                   double upper_limit,
                                                   int buffer_size)
    : tolerance_(tolerance),
      near_zero_(near_zero),
      near_target_(near_target),
      lower_limit_(lower_limit),
      upper_limit_(upper_limit),
      buffer_size_(buffer_size) {
    if (near_zero < 0.0) {
        throw std::runtime_error("RegulatedParameterConfig near_zero < 0");
    }
    if (near_target < 0.0) {
        throw std::runtime_error("RegulatedParameterConfig near_target < 0");
    }
    if (lower_limit < 0.0) {
        throw std::runtime_error("RegulatedParameterConfig lower_limit < 0");
    }
    if (upper_limit < 0.0) {
        throw std::runtime_error("RegulatedParameterConfig upper_limit < 0");
    }
    if (buffer_size < 1) {
        throw std::runtime_error("RegulatedParameterConfig buffer size < 1");
    }
}

bool operator==(const RegulatedParameterConfig &c1, const RegulatedParameterConfig& c2) {
    return (std::abs(c1.tolerance() - c2.tolerance()) < 0.0001 &&
            std::abs(c1.near_zero() - c2.near_zero()) < 0.0001 &&
            std::abs(c1.near_target() - c2.near_target()) < 0.0001 &&
            std::abs(c1.lower_limit() - c2.lower_limit()) < 0.0001 &&
            std::abs(c1.upper_limit() - c2.upper_limit()) < 0.0001 &&
            c1.buffer_size() == c2.buffer_size());
}

bool operator!=(const RegulatedParameterConfig &c1, const RegulatedParameterConfig& c2) {
    return !(c1 == c2);
}

}
