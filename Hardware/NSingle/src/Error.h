#ifndef NSINGLE_ERROR_H
#define NSINGLE_ERROR_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>
#include <QStringList>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT Error
{
    Q_GADGET

public:
    enum Type { OVER_FLOW = 1,
                INTERNAL_FRAME = 2,
                COMMUNICATION = 4,
                I_SETPOINT_NOT_ZERO = 8,
                I_VALUE_NOT_ZERO = 16,
                CMD_ON_ACTIVE = 32,
                POLARITY_CHANGE_FORBIDDEN = 64,
                STATE_CHANGE = 128 };
    Q_ENUM(Type)

    Error(std::bitset<8> bits);
    Error(QString hex);
    Error(const Error& other);
    Error(Error&& other);
    Error& operator=(Error other);
    friend void swap(Error& first, Error& second) {
        using std::swap;
        swap(first.data_, second.data_);
    }

    static QMap<QString, Type> types();
    static QString TypeToString(Type type);

    QStringList active() const;
    QString hexcode() const;
    std::bitset<8> bitset() const;

private:
    friend bool operator==(const Error &c1, const Error& c2);
    quint8 data_;
};

bool operator==(const Error &c1, const Error& c2);
bool operator!=(const Error &c1, const Error& c2);

} // namespace

#endif
