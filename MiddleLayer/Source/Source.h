#ifndef MIDDLELAYER_SOURCE_H
#define MIDDLELAYER_SOURCE_H

#include <QSettings>
#include <QStateMachine>

#include "nsinglecontroller_interface.h"
#include "sourcepowersupply_interface.h"
#include "messagelogger_interface.h"

#include "EquipmentGroupState.h"
#include "GlobalRepo.h"
#include "SourcePowerSupplyRepo.h"
#include "NSingleRepo.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::source {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;
namespace hw_sourcepowersupply = medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply;

class Source : public QObject
{
    Q_OBJECT

public:
    explicit Source();
    ~Source();

public slots:
    void Configure(QString cyclo_config); // dbus
    void QueryConfiguration();
    void Startup(); // dbus
    void Shutdown(); // dbus
    void Interrupt(); // dbus    

private slots:
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

    void ExecuteConfiguration();
    void ConfigureInjection33kev();
    void ConfigureExtraction();
    void ConfigureArc();
    void ConfigureFilament();

signals:
    // public
    void SIGNAL_Configuration(QString cyclo_config); // dbus
    void SIGNAL_State_Off(); // dbus
    void SIGNAL_State_Ready(); // dbus
    void SIGNAL_State_Intermediate(); // dbus
    void SIGNAL_State_Unknown(); // dbus
    void SIGNAL_StartupFinished(); // dbus
    void SIGNAL_ShutdownFinished(); // dbus

    // private
    void SIGNAL_CommandConfigure();
    void SIGNAL_CommandStartup();
    void SIGNAL_CommandShutdown();
    void SIGNAL_CommandInterrupt();
    void SIGNAL_Interrupt();
    void SIGNAL_DbusOk();
    void SIGNAL_DbusError();

private:
    void SetupDbusConnections();
    void CheckDbusConnections();
    void SetupGroupState();
    void SetupStateMachine();
    void GetLastActiveConfig();
    void PrintStateChanges(QAbstractState *state, QString name);

    const global::Enumerations::SubSystem sub_system_ = global::Enumerations::SubSystem::SOURCE;

     medicyc::cyclotron::NSingleControllerInterface* injection_ = nullptr;
     medicyc::cyclotron::SourcePowerSupplyInterface* extraction_ = nullptr;
     medicyc::cyclotron::SourcePowerSupplyInterface* arc_ = nullptr;
     medicyc::cyclotron::SourcePowerSupplyInterface* filament_ = nullptr;
     medicyc::cyclotron::MessageLoggerInterface logger_;

     QStateMachine sm_;
     QState ping_dbus_;

     global::GlobalRepo global_repo_;
     hw_nsingle::NSingleRepo nsingle_repo_;
     hw_sourcepowersupply::SourcePowerSupplyRepo source_repo_;
     utils::EquipmentGroupState group_state_;

     QString active_cyclo_config_ = "-";
     QString demanded_cyclo_config_ = "-";
};

}

#endif
