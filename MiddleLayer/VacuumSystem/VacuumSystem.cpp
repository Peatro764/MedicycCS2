#include "VacuumSystem.h"

#include <QStandardPaths>
#include <QSignalTransition>
#include <QDateTime>

#include "DBus.h"
#include "Parameters.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::vacuumsystem {

namespace global = medicyc::cyclotroncontrolsystem::global;

VacuumSystem::VacuumSystem(QStringList channels) :
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    ior_("medicyc.cyclotron.hardware.ior", "/IOR", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "VacuumSystem::VacuumSystem Channels " << channels;
    for (auto& c : channels) {
        channel_states_[c] = false;
    }

    QObject::connect(&ior_, &medicyc::cyclotron::IORInterface::SIGNAL_ReceivedChannelValue, this, &VacuumSystem::InterpretIORMessage);

    QTimer *timer_read_ior = new QTimer;
    timer_read_ior->setInterval(read_ior_interval_);
    timer_read_ior->setSingleShot(false);
    timer_read_ior->start();
    QObject::connect(timer_read_ior, &QTimer::timeout, this, &VacuumSystem::ReadIORChannels);

    QTimer *timer_read_adc = new QTimer;
    timer_read_adc->setInterval(read_adc_interval_);
    timer_read_adc->setSingleShot(false);
    timer_read_adc->start();
    QObject::connect(timer_read_adc, &QTimer::timeout, this, &VacuumSystem::ReadJauges);

    CheckGlobalState();
}

VacuumSystem::~VacuumSystem()
{
}

void VacuumSystem::ReadJauges() {
    if (adc_.isValid()) {
        for (auto &j : jauges_) {
            adc_.Read(j);
        }
    } else {
        qWarning() << "VacuumSystem::ReadJauges No DBUS connection to ADC";
    }
}

void VacuumSystem::ReadIORChannels() {
    if (ior_.isValid()) {
        for (auto& c : channel_states_.keys()) {
            ior_.ReadChannelValue(c);
        }
    } else {
        qWarning() << "VacuumSystem::ReadChannels Dbus not connected";
    }
}

void VacuumSystem::CheckGlobalState() {
    bool atleast_one_off = channel_states_.values().contains(false);
    bool atleast_one_on = channel_states_.values().contains(true);
    if (atleast_one_off && atleast_one_on) {
        emit SIGNAL_VacuumPartiallyOk();
    } else if (atleast_one_off) {
        emit SIGNAL_AllVacuumError();
    } else {
        emit SIGNAL_AllVacuumOk();
    }
}

void VacuumSystem::InterpretIORMessage(const QString& channel, bool state) {
    if (channel_states_.contains(channel)) {
        emit SIGNAL_ChannelState(channel, state);
        if (channel_states_[channel] != state) {
            channel_states_[channel] = state;
            if (state) {
                InfoMessage(channel + " ok");
            } else {
                ErrorMessage(channel + " pas bonne");
            }
        }
        CheckGlobalState();
    } else {
        qWarning() << "VacuumSystem::InterpretIORChannel No such channel: " << channel;
    }
}

void VacuumSystem::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Vide"), message);
}

void VacuumSystem::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Vide"), message);
}

void VacuumSystem::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), QString("Vide"), message);
}



}
