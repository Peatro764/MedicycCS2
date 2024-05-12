#ifndef MIDDLELAYER_COOLINGSYSTEM_H
#define MIDDLELAYER_COOLINGSYSTEM_H

#include <QSettings>
#include <QStateMachine>

#include "Enumerations.h"
#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::coolingsystem {


class CoolingSystem : public QObject
{
    Q_OBJECT

public:
    explicit CoolingSystem(QStringList channels);
    ~CoolingSystem();

public slots:

private slots:
    void ReadChannels();
    void CheckGlobalState();
    void InterpretOmronBitRead(const QString& channel, bool content);

signals:
    void SIGNAL_ChannelState(QString name, bool state); // dbus
    void SIGNAL_AllCoolingOn();  // dbus
    void SIGNAL_AllCoolingOff();  // dbus
    void SIGNAL_PartiallyOn(); // dbus

private:
    const global::Enumerations::SubSystem sub_system_ = global::Enumerations::SubSystem::COOLINGSYSTEM;

     medicyc::cyclotron::MessageLoggerInterface logger_;
     medicyc::cyclotron::ADCInterface adc_;
     medicyc::cyclotron::OmronInterface omron_;
     QMap<QString, bool> channel_states_;
};

}

#endif
