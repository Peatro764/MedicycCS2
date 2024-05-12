#include "Bit.h"


namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

bool operator==(const Bit &c1, const Bit& c2) {
    return (c1.enabled_ == c2.enabled_ &&
            c1.logic_ == c2.logic_ &&
            c1.index_ == c2.index_);
}

bool operator!=(const Bit &c1, const Bit& c2) {
    return !(c1 == c2);
}

} // ns
