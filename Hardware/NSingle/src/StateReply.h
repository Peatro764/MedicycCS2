#ifndef NSINGLE_STATEREPLY_H
#define NSINGLE_STATEREPLY_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>

#include "NSingleUtil.h"
#include "Bit.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

enum class StateReplyChannel { StdBy = 0, Off = 1, On = 2, Ok = 3, Error = 4, Local = 5, Remote = 6, Switch1 = 7, Switch2 = 8 };

class NSINGLE_EXPORT StateReplyConfig {
public:
    StateReplyConfig(Bit stdby, Bit off, Bit on, Bit ok,
                     Bit error, Bit local, Bit remote, Bit switch1, Bit switch2) {
        bits_[StateReplyChannel::StdBy] = stdby;
        bits_[StateReplyChannel::Off] = off;
        bits_[StateReplyChannel::On] = on;
        bits_[StateReplyChannel::Ok] = ok;
        bits_[StateReplyChannel::Error] = error;
        bits_[StateReplyChannel::Local] = local;
        bits_[StateReplyChannel::Remote] = remote;
        bits_[StateReplyChannel::Switch1] = switch1;
        bits_[StateReplyChannel::Switch2] = switch2;
    }

    bool enabled(StateReplyChannel ch) const { return bits_[ch].enabled(); }
    bool logic(StateReplyChannel ch) const { return bits_[ch].logic(); }
    int index(StateReplyChannel ch) const { return bits_[ch].index(); }
private:
    friend bool operator==(const StateReplyConfig &c1, const StateReplyConfig& c2);
    QMap<StateReplyChannel, Bit> bits_;
};

class NSINGLE_EXPORT StateReply
{
    Q_GADGET

public:
    StateReply(StateReplyConfig config);
    StateReply(const StateReply& other);
    StateReply(StateReply&& other);
    StateReply& operator=(StateReply other);
    friend void swap(StateReply& first, StateReply& second) {
        using std::swap;
        swap(first.data_, second.data_);
        swap(first.config_, second.config_);
    }

    void SetData(QString hex);
    void SetData(std::bitset<8> bits);
    bool Channel(StateReplyChannel ch) const;
    QString hexcode() const;
    std::bitset<8> bitset() const;

private:
    friend bool operator==(const StateReply &c1, const StateReply& c2);
    StateReplyConfig config_;
    quint8 data_;
};

bool operator==(const StateReplyConfig &c1, const StateReplyConfig& c2);
bool operator!=(const StateReplyConfig &c1, const StateReplyConfig& c2);

bool operator==(const StateReply &c1, const StateReply& c2);
bool operator!=(const StateReply &c1, const StateReply& c2);

} // namespace

#endif
