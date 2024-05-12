#include "VirtualOmron.h"

#include <unistd.h>
#include <iomanip>
#include <sstream>
#include <QDebug>
#include <QStandardPaths>

#include "MemoryAreaCommand.h"
#include "NodeAddressCommand.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::simulators::omron {

VirtualOmron::VirtualOmron()
{
}

VirtualOmron::~VirtualOmron() {
}

void VirtualOmron::SetChannels(const QMap<QString, hw_omron::Channel>& channels) {
    channels_ = channels;
    foreach(hw_omron::Channel channel, channels.values()) {
        QString hash = HashedAddress(channel.node(), channel.fins_memeory_area(), channel.address(), channel.bit());
        hashed_channels_[hash] = channel;
    }
}

void VirtualOmron::SetConnected(bool yes) {
    connected_ = yes;
    Ping();
}

void VirtualOmron::Ping() {
    if (connected_)
        emit SIGNAL_Connected();
    else
        emit SIGNAL_Disconnected();
}

// only bit implemented so far
void VirtualOmron::ReadChannel(QString name) {
    if (ChannelExist(name)) {
        emit SIGNAL_ReadRequest(name);
    } else {
        qWarning() << "VirtualOmron::ReadChannel " << name;
    }
}

// only bit implemented so far
void VirtualOmron::WriteChannel(QString name, bool content) {
    if (ChannelExist(name)) {
        hw_omron::Channel ch = GetChannel(name);
        if (ch.impuls()) {
            emit SIGNAL_WriteRequest(name, false);
            emit SIGNAL_WriteRequest(name, true);
            emit SIGNAL_WriteRequest(name, false);
        } else {
            emit SIGNAL_WriteRequest(name, content);
        }
    } else {
        qWarning() << "VirtualOmron::WriteChannel unknown " << name;
    }
}

// THROWS
hw_omron::Channel VirtualOmron::GetChannel(QString name) const {
    if (!channels_.contains(name)) {
        throw std::runtime_error("No such channel");
    }
    return channels_.value(name);
}

// use this one before calling GetChannel
bool VirtualOmron::ChannelExist(QString name) const {
    return channels_.contains(name);
}

// THROWS
hw_omron::Channel VirtualOmron::GetChannel(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const {
    QString hash = HashedAddress(node, memory_area, address, bit);
    if (!hashed_channels_.contains(hash)) {
        throw std::runtime_error("No such channel");
    }
    return hashed_channels_.value(hash);
}

// use this one before calling GetChannel
bool VirtualOmron::ChannelExist(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const {
    QString hash = HashedAddress(node, memory_area, address, bit);
    return hashed_channels_.contains(hash);
}

}

