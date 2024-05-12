#ifndef HFUI_H
#define HFUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>

#include "ISubSystemUI.h"
#include "HFUtils.h"
#include "HFConfigurationUI.h"
#include "HFPowerSupplyDetailedUI.h"
#include "hfpowersupplies_interface.h"
#include "hfcontroller_interface.h"

class QWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class GenericDisplayButton;

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace middlelayer = medicyc::cyclotron::middlelayer;

class HFUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    HFUI(QWidget *parent);
    ~HFUI();

public slots:
    void Configure(QString name) override; // not used
    void Startup() override;
    void Shutdown() override;
    void Interrupt() override;

signals:
    void SIGNAL_StartupFinished() override;
    void SIGNAL_ShutdownFinished() override;
    void SIGNAL_State_Off() override;
    void SIGNAL_State_Intermediate() override;
    void SIGNAL_State_Ready() override;
    void SIGNAL_State_Unknown() override;
    void SIGNAL_FrequencyChanged();

private slots:
    void StartupAfterConfirmation();
    void ShutdownAfterConfirmation();
    void OpenDetailedPowerSupplyGui();
    void OpenConfigurationGui();

private:
    void SetupUI();
    QHBoxLayout* CreateTitleWindow();
    QVBoxLayout* CreatePowerSuppliesStatusLayout();
    QHBoxLayout* CreatePowerSuppliesCommandLayout();
    QFrame* CreateNiveauWindow(medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX hf);
    QVBoxLayout* CreateGeneratorWindow();

    medicyc::cyclotron::HFPowerSuppliesInterface powersupplies_interface_;
    middlelayer::HFControllerInterface controller_interface_;
    HFPowerSupplyDetailedUI detailed_ui_;
    HFConfigurationUI configuration_ui_;
};

}

#endif
