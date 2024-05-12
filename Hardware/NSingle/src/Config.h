#ifndef NSINGLE_CONFIG_H
#define NSINGLE_CONFIG_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT Config
{
    Q_GADGET

public:
    // The masks indicate where in the bitset the config types are
    enum Masks { KIND = 7, POLARITY = 8, FILTERING = 48, AVERAGING = 64, STATEMONITORING = 128 };
    // The values indicate the value that corresponds to the state in question, provided the bitset has been masked with the above mask
    enum Kind { Basic = 0, BP = 1, BC_ISR = 2, VDF = 4 };
    enum Polarity { Uni = 0, Bi = 8 };
    enum Filtering { None = 0, Low = 16, Medium = 32, High = 48 };
    enum Averaging { AbsoluteValues = 0, SignedValues = 64 };
    enum StateMonitoring { OFF = 0, ON = 128 };
    Q_ENUM(Kind)
    Q_ENUM(Polarity)
    Q_ENUM(Filtering)
    Q_ENUM(Averaging)
    Q_ENUM(StateMonitoring)

    Config(QString hex);
    Config(std::bitset<8> bits);
    Config(Kind kind, Polarity polarity, Filtering filtering, Averaging averaging, StateMonitoring statemonitoring);
    Config(const Config& other);
    Config(Config&& other);
    Config& operator=(Config other);
    friend void swap(Config& first, Config& second) {
        using std::swap;
        swap(first.data_, second.data_);
    }

    static QMap<QString, Kind> kinds();
    static QMap<QString, Polarity> polarities();
    static QMap<QString, Filtering> filterings();
    static QMap<QString, Averaging> averagings();
    static QMap<QString, StateMonitoring> statemonitorings();

    static QString KindToString(Kind kind);
    static QString PolarityToString(Polarity polarity);
    static QString FilteringToString(Filtering filtering);
    static QString AveragingToString(Averaging averaging);
    static QString StateMonitoringToString(StateMonitoring statemonitoring);

    Kind kind() const;
    Polarity polarity() const;
    Filtering filtering() const;
    Averaging averaging() const;
    StateMonitoring statemonitoring() const;

    void SetKind(Kind kind);
    void SetPolarity(Polarity polarity);
    void SetFiltering(Filtering filtering);
    void SetAveraging(Averaging averaging);
    void SetStateMonitoring(StateMonitoring statemonitoring);

    QString hexcode() const;
    std::bitset<8> bitset() const;

private:
    friend bool operator==(const Config &c1, const Config& c2);
    quint8 data_;
};

bool operator==(const Config &c1, const Config& c2);
bool operator!=(const Config &c1, const Config& c2);

} // namespace

#endif
