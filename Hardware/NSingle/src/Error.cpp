#include "Error.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

Error::Error(QString hex) {
    if (hex.size() != 2) { throw std::runtime_error("Error hex code size wrong"); }

    bool ok(false);
    quint8 dec = hex.toUInt(&ok, 16);
    if (!ok) { throw std::runtime_error("Error string not valid hex code"); }

    data_ = dec;
}

Error::Error(std::bitset<8> bits)
    : data_(static_cast<quint8>(bits.to_ulong())) {}

Error::Error(const Error& other) :
    data_(other.data_) {}

Error::Error(Error&& other)
    : data_{other.data_} {}

Error& Error::operator=(Error other) {
    swap(*this, other);
    return *this;
}

QMap<QString, Error::Type> Error::types() {
    QMap<QString, Type> types;
    QMetaEnum e = QMetaEnum::fromType<Error::Type>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Type type = static_cast<Type>(e.value(key));
        types[TypeToString(type)] = type;
    }
    return types;
}

QString Error::TypeToString(Type type) {
    return QMetaEnum::fromType<Error::Type>().valueToKey(type);
}

QStringList Error::active() const {
    QStringList list;
    QMetaEnum e = QMetaEnum::fromType<Error::Type>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Type type = static_cast<Type>(e.value(key));
         if ((data_ & static_cast<quint8>(type)) == static_cast<quint8>(type)) {
             list.append(TypeToString(type));
         }
    }
    return list;
}

QString Error::hexcode() const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data_;
    return QString("%1").arg(QString(buffer.toHex()), 2, QLatin1Char('0')).toUpper();
}

std::bitset<8> Error::bitset() const {
    return std::bitset<8>(data_);
}

bool operator==(const Error &c1, const Error& c2) {
    return (c1.data_ == c2.data_);
}

bool operator!=(const Error &c1, const Error& c2) {
    return !(c1 == c2);
}

} // ns
