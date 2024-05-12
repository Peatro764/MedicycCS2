#ifndef NSINGLEDBUSADDRESS_H
#define NSINGLEDBUSADDRESS_H

#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSingleDbusAddress {
public:
    NSingleDbusAddress() {}
    NSingleDbusAddress(QString nsingle, QString interface_address, QString controller_object_name, QString nsingle_object_name)
        : nsingle_(nsingle), interface_address_(interface_address), controller_object_name_(controller_object_name), nsingle_object_name_(nsingle_object_name)
    {}

    QString nsingle() const { return nsingle_; }
    QString interface_address() const { return interface_address_; }
    QString controller_object_name() const { return controller_object_name_; }
    QString nsingle_object_name() const { return nsingle_object_name_; }
private:
    QString nsingle_;
    QString interface_address_;
    QString controller_object_name_;
    QString nsingle_object_name_;
};

}

#endif // NSINGLEDBUSADDRESS_H
