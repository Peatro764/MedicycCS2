#include "StateReply.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

StateReply::StateReply(StateReplyConfig config)
    : config_(config) {}

StateReply::StateReply(const StateReply& other) :
    config_(other.config_), data_(other.data_) {}

StateReply::StateReply(StateReply&& other)
    : config_(other.config_), data_{other.data_} {}

StateReply& StateReply::operator=(StateReply other) {
    swap(*this, other);
    return *this;
}

void StateReply::SetData(QString hex) {
    if (hex.size() != 2) { throw std::runtime_error("StateReply hex code size wrong"); }

    bool ok(false);
    quint8 dec = hex.toUInt(&ok, 16);
    if (!ok) { throw std::runtime_error("StateReply string not valid hex code"); }

    data_ = dec;
}

void StateReply::SetData(std::bitset<8> bits) {
    data_ = bits.to_ulong();
}

bool StateReply::Channel(StateReplyChannel ch) const {
    if (!config_.enabled(ch)) return false;
    const int bit = config_.index(ch);
    return ((data_ & (0x1 << bit)) >> bit) == config_.logic(ch);
}

QString StateReply::hexcode() const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data_;
    return QString("%1").arg(QString(buffer.toHex()), 2, QLatin1Char('0')).toUpper();
}

std::bitset<8> StateReply::bitset() const {
    return std::bitset<8>(data_);
}

bool operator==(const StateReplyConfig &c1, const StateReplyConfig& c2) {
    return (c1.bits_ == c2.bits_);
}

bool operator!=(const StateReplyConfig &c1, const StateReplyConfig& c2) {
    return !(c1 == c2);
}

bool operator==(const StateReply &c1, const StateReply& c2) {
    return (c1.data_ == c2.data_ &&
            c1.config_ == c2.config_);
}

bool operator!=(const StateReply &c1, const StateReply& c2) {
    return !(c1 == c2);
}

} // ns
