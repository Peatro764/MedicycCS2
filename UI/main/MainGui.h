#ifndef MAINGUI_H
#define MAINGUI_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include <QStateMachine>

#include "SourceUI.h"
#include "HFUI.h"
#include "IOLoadUI.h"
#include "BobinePrincipaleUI.h"
#include "NSingleGroupUI.h"
#include "ProcessMonitorUI.h"
#include "MessageUI.h"
#include "CoolingUI.h"
#include "VacuumUI.h"
#include "GlobalRepo.h"
#include "SoundMessages.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace middlelayer = medicyc::cyclotron::middlelayer;

class MainGui : public QWidget
{
    Q_OBJECT

public:
    MainGui();
    ~MainGui();

public slots:

private slots:
    void OpenPrintDialog();
    void StartDetachedProcess(QString name, QStringList args);

signals:
    void SIGNAL_GlobalStartup();
    void SIGNAL_GlobalShutdown();
    void SIGNAL_GlobalInterrupt();

private:
    void SetupUI();
    void SetupGlobalStartupSM();
    void SetupGlobalCommands(QBoxLayout*);
    QFrame* CreateGlobalStatusFrame();
    QFrame* CreateConfigurationFrame();
    QFrame* CreateMiscCommands();

    void SetupHF();
    void SetupIOLoad();
    void SetupVoiceMessages();

    void PrintStateChanges(QState *state, QString name);

    medicyc::cyclotroncontrolsystem::global::GlobalRepo global_repo_;

    // subsystem ui's
    ProcessMonitorUI process_monitor_ui_;
    IOLoadUI io_load_;
    SourceUI source_ui_;
    NSingleGroupUI injection_nsingle_group_;
    BobinePrincipaleUI bp_group_;
    NSingleGroupUI bc_nsingle_group_;
    NSingleGroupUI ejection_nsingle_group_;
    HFUI hf_;
    NSingleGroupUI vdf_nsingle_group_;
    CoolingUI cooling_ui_;
    VacuumUI vacuum_ui_;
    MessageUI message_ui_;
    SoundMessages sound_messages_;

    QStateMachine sm_global_startup_;
};

}

#endif
