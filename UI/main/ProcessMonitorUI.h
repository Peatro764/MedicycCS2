#ifndef PROCESSMONITORUI_H
#define PROCESSMONITORUI_H

#include <QString>
#include <QFont>
#include <QMap>
#include <QWidget>

#include "launcher_interface.h"
#include "messagelogger_interface.h"

class QPushButton;
class QLabel;
class QGridLayout;
class GenericDisplayButton;

namespace medicyc::cyclotroncontrolsystem::ui::main {

struct ProcessUIElements {
    QLabel *name;
    QPushButton *toggle;
    GenericDisplayButton *allumee;
};

class ProcessMonitorUI : public QWidget
{
    Q_OBJECT
public:
    ProcessMonitorUI(QWidget *parent = 0);
    ~ProcessMonitorUI();
private slots:
    void ProcessCreated(const QString& name);
    void ProcessStarted(QString name);
    void ProcessStopped(QString name);

signals:
    void SIGNAL_ToggleProcessState(QString name);
    void SIGNAL_RequiredProcessesRunning(bool yes);

private:
    ProcessUIElements* GetProcessUIElements(QString name);
    QMap<QString, ProcessUIElements*> uielements_;
    QGridLayout *grid_layout_;
    const int GRIDLAYOUTMAXNROWS = 8;
    int grid_layout_active_row_ = 0;
    int grid_layout_active_col_ = 0;

    medicyc::cyclotron::LauncherInterface launcher_interface_;
    medicyc::cyclotron::MessageLoggerInterface messagelogger_interface_;
};

}
#endif

