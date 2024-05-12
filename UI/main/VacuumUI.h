#ifndef VACUUMGUI_H
#define VACUUMGUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
#include <map>

#include "IORRepo.h"
#include "ISubSystemUI.h"
#include "Style.h"
#include "GenericDisplayButton.h"
#include "vacuumsystem_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class VacuumUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    VacuumUI(QWidget *parent);
    ~VacuumUI();
    bool AllOk() const { return all_ok_; }

public slots:
    void Configure(QString name) override; // not used
    void Startup() override; // not used
    void Shutdown() override; // not used
    void Interrupt() override; // not used

signals:
    void SIGNAL_State_Off() override;
    void SIGNAL_State_Intermediate() override;
    void SIGNAL_State_Ready() override;
    void SIGNAL_State_Unknown() override;
    void SIGNAL_StartupFinished() override; // not used
    void SIGNAL_ShutdownFinished() override; // not used

private slots:
    void UpdateChannelState(const QString& channel, bool state);
    void CreateChannels();
    QGridLayout* SetupStatusIcons();

    void SetPartiallyOn();
    void SetAllOn();
    void SetAllOff();

private:
    medicyc::cyclotron::middlelayer::VacuumSystemInterface interface_;
    medicyc::cyclotroncontrolsystem::hardware::ior::IORRepo repo_;
    std::map<QString, GenericDisplayButton*> gauges_;
    bool all_ok_ = false;
};

}

#endif
