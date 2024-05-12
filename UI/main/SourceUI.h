#ifndef SOURCEUI_H
#define SOURCEUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QVBoxLayout>

#include "ISubSystemUI.h"
#include "source_interface.h"
#include "sourcepowersupply_interface.h"
#include "nsinglecontroller_interface.h"
#include "SourcePowerSupplyRepo.h"
#include "NSingleRepo.h"
#include "EquipmentGroupValues.h"

class QWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class GenericDisplayButton;
class PoleLevelIndicator;

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace middlelayer = medicyc::cyclotron::middlelayer;
namespace hw_source = medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply;
namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class PowerSupplyUIElements {
public:
    PowerSupplyUIElements() {}
    ~PowerSupplyUIElements() {}    
    QLabel* name = nullptr;
    QPushButton* startup = nullptr;
    QPushButton* shutdown = nullptr;
    QLabel* act_voltage = nullptr;
    QLabel* act_current = nullptr;
    QString des_voltage = "NA";
    QString des_current = "NA";
    GenericDisplayButton* connected = nullptr;
    PoleLevelIndicator* io_load = nullptr;
    GenericDisplayButton* switched_on = nullptr;
    GenericDisplayButton* state = nullptr;
    GenericDisplayButton* ontarget = nullptr;
};

class SourceUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    SourceUI(QWidget *parent);
    ~SourceUI();
    EquipmentGroupValues GetGroupValues() const;

public slots:
    void Configure(QString name) override;
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
    void SIGNAL_InjectionCurrent(double value);

private slots:
    void StartupAfterConfirmation();
    void ShutdownAfterConfirmation();

private:
    void SetupUI();
    void CreatePowerSupplies();
    // Source power supplies
    PowerSupplyUIElements* CreateSourcePowerSupplyUIElements(QString name);
    void ConnectSourcePowerSupplyUIElements(QString name);
    QVBoxLayout* CreateSourcePowerSupplyLayout(QString name);
    // Injection power supply
    PowerSupplyUIElements* CreateNSinglePowerSupplyUIElements(QString name);
    void ConnectNSinglePowerSupplyUIElements(QString name);
    QVBoxLayout* CreateNSinglePowerSupplyLayout(QString name);
    void SetupInjectionCurrentPolling();

    hw_source::SourcePowerSupplyRepo source_repo_;
    hw_nsingle::NSingleRepo nsingle_repo_;

    middlelayer::SourceInterface source_interface_;

    QMap<QString, QString> powersupply_primary_param_;
    QMap<QString, PowerSupplyUIElements*> powersupply_uielements_;
    QMap<QString, hw_source::PowerSupplyConfig> sourcepowersupply_configs_;
    QMap<QString, medicyc::cyclotron::SourcePowerSupplyInterface*> sourcepowersupply_interfaces_;
    QMap<QString, medicyc::cyclotron::NSingleControllerInterface*> nsingle_interfaces_;
};

}

#endif
