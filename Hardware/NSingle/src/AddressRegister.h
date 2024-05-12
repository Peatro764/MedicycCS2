#ifndef NSINGLE_ADDRESSREGISTER_H
#define NSINGLE_ADDRESSREGISTER_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>
#include <QStringList>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT AddressRegister
{
    Q_GADGET

public:
    AddressRegister(std::bitset<8> bits);
    AddressRegister(QString hex);

    QString hexcode() const;
    std::bitset<8> bitset() const;

private:
    friend bool operator==(const AddressRegister &c1, const AddressRegister& c2);
    quint8 data_;
};

bool operator==(const AddressRegister &c1, const AddressRegister& c2);
bool operator!=(const AddressRegister &c1, const AddressRegister& c2);

} // namespace

#endif
