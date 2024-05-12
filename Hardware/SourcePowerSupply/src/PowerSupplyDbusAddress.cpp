#include "PowerSupplyDbusAddress.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

bool operator==(const PowerSupplyDbusAddress &c1, const PowerSupplyDbusAddress& c2) {
    return (c1.power_supply() == c2.power_supply() &&
            c1.interface_address() == c2.interface_address() &&
            c1.object_name() == c2.object_name());
}

bool operator!=(const PowerSupplyDbusAddress &c1, const PowerSupplyDbusAddress& c2) {
    return !(c1 == c2);
}

} // ns
