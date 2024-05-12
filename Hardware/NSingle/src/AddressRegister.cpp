#include "AddressRegister.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

AddressRegister::AddressRegister(QString hex) {
    if (hex.size() != 2) { throw std::runtime_error("AddressRegister hex code size wrong"); }

    bool ok(false);
    quint8 dec = hex.toUInt(&ok, 16);
    if (!ok) { throw std::runtime_error("AddressRegister string not valid hex code"); }

    data_ = dec;
}

AddressRegister::AddressRegister(std::bitset<8> bits)
    : data_(static_cast<quint8>(bits.to_ulong())) {}

QString AddressRegister::hexcode() const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data_;
    return QString("%1").arg(QString(buffer.toHex()), 2, QLatin1Char('0')).toUpper();
}

std::bitset<8> AddressRegister::bitset() const {
    return std::bitset<8>(data_);
}

bool operator==(const AddressRegister &c1, const AddressRegister& c2) {
    return (c1.data_ == c2.data_);
}

bool operator!=(const AddressRegister &c1, const AddressRegister& c2) {
    return !(c1 == c2);
}

} // ns
