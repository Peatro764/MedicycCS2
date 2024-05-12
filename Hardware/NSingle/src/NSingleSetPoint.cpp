#include "NSingleSetPoint.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

NSingleSetPoint::NSingleSetPoint(bool powered_up, bool sign, double value)
    : powered_up_(powered_up), sign_(sign), value_(value) {

}

NSingleSetPoint::~NSingleSetPoint() {

}

bool operator==(const NSingleSetPoint& c1, const NSingleSetPoint &c2) {
    return (c1.powered_up_ == c2.powered_up() &&
            c1.sign_ == c2.sign_ &&
            std::abs(c1.value_ - c2.value_) < 0.000001);
}

bool operator!=(const NSingleSetPoint& c1, const NSingleSetPoint &c2) {
    return !(c1==c2);
}

}
