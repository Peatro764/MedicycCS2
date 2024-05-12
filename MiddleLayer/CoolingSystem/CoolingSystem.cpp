#include "CoolingSystem.h"

#include <QStandardPaths>
#include <QSignalTransition>
#include <QDateTime>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::coolingsystem {

namespace global = medicyc::cyclotroncontrolsystem::global;

CoolingSystem::CoolingSystem(QStringList channels) :
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.cooling", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "CoolingSystem::CoolingSystem Channels " << channels;
    for (auto& c : channels) {
        channel_states_[c] = false;
    }

    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, &CoolingSystem::InterpretOmronBitRead);
    QTimer *timer_read = new QTimer;
    timer_read->setInterval(5000);
    timer_read->setSingleShot(false);
    timer_read->start();
    QObject::connect(timer_read, &QTimer::timeout, this, &CoolingSystem::ReadChannels);
    CheckGlobalState();
}

CoolingSystem::~CoolingSystem()
{
}

void CoolingSystem::ReadChannels() {
    if (omron_.isValid()) {
        for (auto& c : channel_states_.keys()) {
            omron_.ReadChannel(c);
        }
    } else {
        qWarning() << "CoolingSystem::ReadChannels Omron Dbus not connected";
    }

    if (adc_.isValid()) {
        // This value is not used here but is read so that the datalogger can save it to the db
        // if you want to use it in the control system, capture the reply and emit a signal
        adc_.Read("Temperature Eau");
    } else {
        qWarning() << "CoolingSystem::ReadChannels ADC Dbus not connected";
    }
}

void CoolingSystem::CheckGlobalState() {
    bool atleast_one_off = channel_states_.values().contains(false);
    bool atleast_one_on = channel_states_.values().contains(true);
    if (atleast_one_off && atleast_one_on) {
        emit SIGNAL_PartiallyOn();
    } else if (atleast_one_off) {
        emit SIGNAL_AllCoolingOff();
    } else {
        emit SIGNAL_AllCoolingOn();
    }
}

void CoolingSystem::InterpretOmronBitRead(const QString& channel, bool content) {
    if (channel_states_.contains(channel)) {
        emit SIGNAL_ChannelState(channel, content);
        if (channel_states_[channel] != content) {
            channel_states_[channel] = content;
        }
        CheckGlobalState();
    } else {
        qWarning() << "CoolingSystem::InterpretOmronBitRead No such channel: " << channel;
    }
}


}
