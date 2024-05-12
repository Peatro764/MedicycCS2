#ifndef COOLINGUI_H
#define COOLINGUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
#include <map>

#include "OmronRepo.h"
#include "ISubSystemUI.h"
#include "Style.h"
#include "GenericDisplayButton.h"
#include "coolingsystem_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class CoolingUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    CoolingUI(QWidget *parent);
    ~CoolingUI();
    bool AllOn() const { return all_on_; }

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
    void CreateValves();
    QGridLayout* SetupStatusIcons();

    void SetPartiallyOn();
    void SetAllOn();
    void SetAllOff();

private:
    medicyc::cyclotron::middlelayer::CoolingSystemInterface interface_;
    medicyc::cyclotroncontrolsystem::hardware::omron::OmronRepo repo_;
    std::map<QString, GenericDisplayButton*> valves_;
    bool all_on_ = false;
};

}

#endif
