#ifndef MIDDLELAYER_VACUUMSYSTEM_H
#define MIDDLELAYER_VACUUMSYSTEM_H

#include <QSettings>
#include <QStateMachine>

#include "Enumerations.h"
#include "ior_interface.h"
#include "adc_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::vacuumsystem {

/*
 * Reads the state of the vacuum in the cyclo, vdf and source through two different channels
 * 1) IOR (digital information wheather vacuum is ok). The global state, based on this info, is signaled to superieur levels through signals
 * 2) ADC The actual values of the jauges measuring the vacuum is sampled. The DataLogger class then makes sure that this information is sent to the db
 * */

class VacuumSystem : public QObject
{
    Q_OBJECT

public:
    explicit VacuumSystem(QStringList channels);
    ~VacuumSystem();

public slots:

private slots:
    void ReadJauges();
    void ReadIORChannels();
    void CheckGlobalState();
    void InterpretIORMessage(const QString& channel, bool state);
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);

signals:
    void SIGNAL_ChannelState(QString name, bool state); // dbus
    void SIGNAL_AllVacuumOk();  // dbus
    void SIGNAL_AllVacuumError();  // dbus
    void SIGNAL_VacuumPartiallyOk(); // dbus

private:
     medicyc::cyclotron::MessageLoggerInterface logger_;
     medicyc::cyclotron::IORInterface ior_;
     medicyc::cyclotron::ADCInterface adc_;
     QMap<QString, bool> channel_states_;
     std::vector<QString> jauges_ { "Jauge Source Pe2", "Jauge Source Pe3",
                                    "Jauge VdF Pe20", "Jauge VdF Pe21", "Jauge VdF Pe24",
                                    "Jauge Cyclo Pe4","Jauge Cyclo Pe5" };
     const int read_ior_interval_ = 10000;
     const int read_adc_interval_ = 30000;
};

}

#endif
