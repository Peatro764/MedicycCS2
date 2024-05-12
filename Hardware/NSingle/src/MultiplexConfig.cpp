#include "MultiplexConfig.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

MultiplexConfig::MultiplexConfig() {}

MultiplexConfig::MultiplexConfig(QMap<QString, int> channels)
    : channels_(channels), it_(channels_.begin())
{
}

bool MultiplexConfig::ChannelExist(QString channel) const {
    return channels_.contains(channel);
}

std::bitset<8> MultiplexConfig::SelectChannel(QString channel) const {
    if (!channels_.contains(channel)) {
        QString msg = QString("The multiplex nsingle %1 does not exist").arg(channel);
        throw std::runtime_error(msg.toStdString());
    }
    int channel_address = channels_.value(channel);
    if (channel_address < 0 || channel_address > 0xFF) {
        throw std::runtime_error("Multiplexed address value out of range");
    }
    return std::bitset<8>(channel_address);
}

QString MultiplexConfig::Name(int address) const {
    return channels_.key(address, QString("NONE"));
}


std::vector<QString> MultiplexConfig::channels() const {
    std::vector<QString> channel_keys;
    foreach(QString ch, channels_.keys()) channel_keys.push_back(ch);
    return channel_keys;
}

void MultiplexConfig::AddChannel(QString name, int address) {
    if (channels_.contains(name)) {
        throw std::runtime_error("Multiplex config already contains this channel");
    }
    channels_[name] = address;
    it_ = channels_.begin();
}

QString MultiplexConfig::Iterate() {
    if (channels_.empty()) {
        qWarning() << "MultiplexConfig::Iterate No channels";
        return QString("NONE");
    }
    ++it_;
    if (it_ == channels_.end()) {
        it_ = channels_.begin();
    }
    return it_.key();
}

bool operator==(const MultiplexConfig &c1, const MultiplexConfig& c2) {
    if (c1.channels() != c2.channels()) {
        return false;
    }
    auto channels = c1.channels();
    for (auto &ch : channels) {
        if (c1.SelectChannel(ch) != c2.SelectChannel(ch)) {
            return false;
        }
    }
    return true;
}

bool operator!=(const MultiplexConfig &c1, const MultiplexConfig& c2) {
    return !(c1 == c2);
}

} // ns
