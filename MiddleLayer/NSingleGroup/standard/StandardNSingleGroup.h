#ifndef MIDDLELAYER_STANDARDNSINGLEGROUP_H
#define MIDDLELAYER_STANDARDNSINGLEGROUP_H

#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QStateMachine>

#include "nsinglecontroller_interface.h"
#include "messagelogger_interface.h"
#include "NSingleRepo.h"
#include "GlobalRepo.h"
#include "NSingleSetPoint.h"
#include "Enumerations.h"
#include "EquipmentGroupState.h"

/***
On startup, all available (available=true in nsingle db table) nsingle controllers are created and connected
On configuration, all listed nsingles are configured to be powered down (enabled=false in equipmentconfig table) or powered up with the given setpoint
An nsingle not listed in the configuration table will be ignored during any actions
***/

namespace medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard {

namespace utils = medicyc::cyclotroncontrolsystem::middlelayer::utils;
namespace global = medicyc::cyclotroncontrolsystem::global;
namespace nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class StandardNSingleGroup : public QObject
{
    Q_OBJECT

public:
    StandardNSingleGroup(QString sub_system, int iteration_delay);
    ~StandardNSingleGroup();

public slots:
    void Configure(QString cyclo_config); // dbus
    void QueryConfiguration();
    void Startup(); // dbus
    void Shutdown(); // dbus
    void Interrupt(); // dbus    

signals:
    // external
    void SIGNAL_Configuration(QString cyclo_config);
    void SIGNAL_State_Off();
    void SIGNAL_State_Ready();
    void SIGNAL_State_Intermediate();
    void SIGNAL_State_Unknown();
    void SIGNAL_ShutdownFinished();
    void SIGNAL_StartupFinished();

    // internal
    void SIGNAL_CommandStartup();
    void SIGNAL_CommandShutdown();
    void SIGNAL_CommandInterrupt();
    void SIGNAL_CommandConfigure();
    void SIGNAL_Iterate();
    void SIGNAL_IterationFinished();

private slots:
    void GetLastActiveConfig();
    void StartupIteratively();
    void ShutdownIteratively();
    void SetupControllers();
    void ApplyConfiguration();

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);

private:
    void UnConfigure();
    bool IsConfigured() const;
    void SetupStateMachine();
    void PrintStateChanges(QState *state, QString name);

     QStateMachine sm_;
     QMap<QString, medicyc::cyclotron::NSingleControllerInterface*> nsingle_controllers_;
     QMap<QString, nsingle::NSingleSetPoint> nsingle_setpoints_;
     QMap<QString, nsingle::NSingleSetPoint>::iterator iter_;
     utils::EquipmentGroupState group_state_;

     QString active_cyclo_config_;
     QString demanded_cyclo_config_;
     global::GlobalRepo global_repo_;
     nsingle::NSingleRepo nsingle_repo_;
     medicyc::cyclotron::MessageLoggerInterface logger_;
     QString sub_system_;
     int iteration_delay_;
};

}

#endif
