#ifndef MIDDLELAYER_BOBINEPRINCIPALECYCLER_H
#define MIDDLELAYER_BOBINEPRINCIPALECYCLER_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QStateMachine>
#include <QTimer>

#include "TimedState.h"
#include "NSingleRepo.h"
#include "nsinglecontroller_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::bobine_principale_cycler {

namespace global = medicyc::cyclotroncontrolsystem::global;
namespace nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class BobinePrincipaleCycler : public QObject
{
    Q_OBJECT

public:
    BobinePrincipaleCycler();
    ~BobinePrincipaleCycler();

public slots:
    void Startup(); // dbus
    void Cycle(); // dbus
    void Shutdown(); // dbus
    void Interrupt(); // dbus
    void Continue(); // dbus

signals:
    // external
    void SIGNAL_CyclageStarted();
    void SIGNAL_CyclageFinished();
    void SIGNAL_CyclageInterrupted();
    // status statemachine
    void SIGNAL_State_Off();
    void SIGNAL_State_Ready();
    void SIGNAL_State_Intermediate();
    void SIGNAL_State_Unknown();
    void SIGNAL_ShutdownFinished();
    void SIGNAL_StartupFinished();


    // internal
    void SIGNAL_CommandStartup();
    void SIGNAL_CommandCycle();
    void SIGNAL_CommandShutdown();
    void SIGNAL_CommandInterrupt();
    void SIGNAL_CommandContinue();
    void SIGNAL_Iterate();
    void SIGNAL_IterationFinished();
    void SIGNAL_Error(QString message);

private slots:
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);
    void Iterate();
    void SetCyclingLevels(std::vector<nsingle::BPLevel> levels);

private:
    void SetupEquipment();
    void TearDownEquipment();
    void ConnectSignals();
    void SetupStateStateMachine();
    void SetupCycleStateMachine();
    void PrintStateChanges(QState *state, QString name);

     QStateMachine sm_cycle_;
     QStateMachine sm_state_;
     medicyc::cyclotron::NSingleControllerInterface *controller_ = nullptr;
     nsingle::NSingleRepo nsingle_repo_;
     std::vector<nsingle::BPLevel> bp_levels_;
     std::vector<nsingle::BPLevel>::iterator bp_levels_iter_;
     medicyc::cyclotron::MessageLoggerInterface logger_;
     QTimer timer_sleep_;
     const bool POLARITY = true;
};

}

#endif
