#include "Config.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

Config::Config(QString hex) {
    if (hex.size() != 2) { throw std::runtime_error("Config hex code size wrong"); }

    bool ok(false);
    quint8 dec = hex.toUInt(&ok, 16);
    if (!ok) { throw std::runtime_error("Config string not valid hex code"); }

    data_ = dec;
}

Config::Config(const Config& other) :
    data_(other.data_) {}

Config::Config(Config&& other)
    : data_{other.data_} {}

Config& Config::operator=(Config other) {
    swap(*this, other);
    return *this;
}

Config::Config(std::bitset<8> bits)
    : data_(static_cast<quint8>(bits.to_ulong())) {}

Config::Config(Config::Kind kind,
               Config::Polarity polarity,
               Config::Filtering filtering,
               Config::Averaging averaging,
               Config::StateMonitoring statemonitoring) {
    data_ = static_cast<int>(kind) +
            static_cast<int>(polarity) +
            static_cast<int>(filtering) +
            static_cast<int>(averaging) +
            static_cast<int>(statemonitoring);
}

QMap<QString, Config::Kind> Config::kinds() {
    QMap<QString, Kind> kinds;
    QMetaEnum e = QMetaEnum::fromType<Config::Kind>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Kind kind = static_cast<Kind>(e.value(key));
        kinds[KindToString(kind)] = kind;
    }
    return kinds;
}

QMap<QString, Config::Polarity> Config::polarities() {
    QMap<QString, Polarity> polarities;
    QMetaEnum e = QMetaEnum::fromType<Config::Polarity>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Polarity polarity = static_cast<Polarity>(e.value(key));
        polarities[PolarityToString(polarity)] = polarity;
    }
    return polarities;
}

QMap<QString, Config::Filtering> Config::filterings() {
    QMap<QString, Filtering> filterings;
    QMetaEnum e = QMetaEnum::fromType<Config::Filtering>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Filtering filtering = static_cast<Filtering>(e.value(key));
        filterings[FilteringToString(filtering)] = filtering;
    }
    return filterings;
}

QMap<QString, Config::Averaging> Config::averagings() {
    QMap<QString, Averaging> averagings;
    QMetaEnum e = QMetaEnum::fromType<Config::Averaging>();
    for (int key = 0; key < e.keyCount(); ++key) {
        Averaging averaging = static_cast<Averaging>(e.value(key));
        averagings[AveragingToString(averaging)] = averaging;
    }
    return averagings;
}

QMap<QString, Config::StateMonitoring> Config::statemonitorings() {
    QMap<QString, StateMonitoring> statemonitorings;
    QMetaEnum e = QMetaEnum::fromType<Config::StateMonitoring>();
    for (int key = 0; key < e.keyCount(); ++key) {
        StateMonitoring statemonitoring = static_cast<StateMonitoring>(e.value(key));
        statemonitorings[StateMonitoringToString(statemonitoring)] = statemonitoring;
    }
    return statemonitorings;
}

QString Config::KindToString(Kind kind) {
    return QMetaEnum::fromType<Config::Kind>().valueToKey(kind);
}

QString Config::PolarityToString(Polarity polarity) {
    return QMetaEnum::fromType<Config::Polarity>().valueToKey(polarity);
}

QString Config::FilteringToString(Filtering filtering) {
    return QMetaEnum::fromType<Config::Filtering>().valueToKey(filtering);
}

QString Config::AveragingToString(Averaging averaging) {
    return QMetaEnum::fromType<Config::Averaging>().valueToKey(averaging);
}

QString Config::StateMonitoringToString(StateMonitoring statemonitoring) {
    return QMetaEnum::fromType<Config::StateMonitoring>().valueToKey(statemonitoring);
}

Config::Kind Config::kind() const {
    return static_cast<Config::Kind>(data_ & Masks::KIND);
}

Config::Polarity Config::polarity() const {
    return static_cast<Config::Polarity>(data_ & Masks::POLARITY);
}

Config::Filtering Config::filtering() const {
    return static_cast<Config::Filtering>(data_ & Masks::FILTERING);
}

Config::Averaging Config::averaging() const {
    return static_cast<Config::Averaging>(data_ & Masks::AVERAGING);
}

Config::StateMonitoring Config::statemonitoring() const {
    return static_cast<Config::StateMonitoring>(data_ & Masks::STATEMONITORING);
}

void Config::SetKind(Kind kind) {
    data_ = (data_ & ~Masks::KIND) + static_cast<quint8>(kind);
}

void Config::SetPolarity(Polarity polarity) {
    data_ = (data_ & ~Masks::POLARITY) + static_cast<quint8>(polarity);
}

void Config::SetFiltering(Filtering filtering) {
    data_ = (data_ & ~Masks::FILTERING) + static_cast<quint8>(filtering);
}

void Config::SetAveraging(Averaging averaging) {
    data_ = (data_ & ~Masks::AVERAGING) + static_cast<quint8>(averaging);
}

void Config::SetStateMonitoring(StateMonitoring statemonitoring) {
    data_ = (data_ & ~Masks::STATEMONITORING) + static_cast<quint8>(statemonitoring);
}

QString Config::hexcode() const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data_;
    return QString("%1").arg(QString(buffer.toHex()), 2, QLatin1Char('0')).toUpper();
}

std::bitset<8> Config::bitset() const {
    return std::bitset<8>(data_);
}

bool operator==(const Config &c1, const Config& c2) {
    return (c1.data_ == c2.data_);
}

bool operator!=(const Config &c1, const Config& c2) {
    return !(c1 == c2);
}

} // ns
