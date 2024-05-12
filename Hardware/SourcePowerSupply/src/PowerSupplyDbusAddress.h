#ifndef POWERSUPPLYDBUSADDRESS_H
#define POWERSUPPLYDBUSADDRESS_H

#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

class PowerSupplyDbusAddress {
public:
    PowerSupplyDbusAddress() {}
    PowerSupplyDbusAddress(QString power_supply, QString interface_address, QString object_name)
        : power_supply_(power_supply), interface_address_(interface_address), object_name_(object_name)
    {}

    QString power_supply() const { return power_supply_; }
    QString interface_address() const { return interface_address_; }
    QString object_name() const { return object_name_; }
private:
    QString power_supply_;
    QString interface_address_;
    QString object_name_;
};

bool operator==(const PowerSupplyDbusAddress &c1, const PowerSupplyDbusAddress& c2);
bool operator!=(const PowerSupplyDbusAddress &c1, const PowerSupplyDbusAddress& c2);

}

#endif // SourcePowerSupplyDbusAddress_H
