#ifndef INJECTION_H
#define INJECTION_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QStateMachine>

#include "StandardNSingleGroup.h"
#include "nsingle_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::injection {

namespace global = medicyc::cyclotroncontrolsystem::global;
namespace nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class InjectionNSingleGroup : public QObject
{
    Q_OBJECT

public:
    InjectionNSingleGroup(int iteration_delay);
    ~InjectionNSingleGroup();

public slots:
    void Configure(QString cyclo_config); // dbus
    void QueryConfiguration(); // dbus
    void Startup(); // dbus
    void Shutdown(); // dbus
    void Interrupt(); // dbus

signals:
    void SIGNAL_Configuration(QString cyclo_config);
    void SIGNAL_State_Off(); // dbus
    void SIGNAL_State_Ready(); // dbus
    void SIGNAL_State_Intermediate(); // dbus
    void SIGNAL_State_Unknown(); // dbus
    void SIGNAL_StartupFinished(); // dbus
    void SIGNAL_ShutdownFinished(); // dbus

    // internal
    void SIGNAL_Startup();
    void SIGNAL_Shutdown();
    void SIGNAL_Interrupt();

private slots:
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);

private:
    void SetupSteerer();
    void ConnectSignals();
    void SetupStateMachine();
    void PrintStateChanges(QState *state, QString name);

     QStateMachine sm_;
     int iteration_delay_;
     nsingle::NSingleRepo repo_;
     medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup nsingle_group_;
     medicyc::cyclotron::NSingleInterface* steerer_nsingle_ = nullptr;
     medicyc::cyclotron::MessageLoggerInterface logger_;
};

}

#endif
