#ifndef TOUCH1GUI_H
#define TOUCH1GUI_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QStandardPaths>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMap>
#include <QTimer>
#include <QStackedLayout>

#include "nsingle_interface.h"
#include "nsinglecontroller_interface.h"
#include "sourcepowersupply_interface.h"
#include "NSingleRepo.h"
#include "SourcePowerSupplyRepo.h"
#include "PowerSupplyConfig.h"

#ifdef RASPBERRY_PI
#include "GPIOHandler.h"
#endif

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;
namespace hw_source = medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply;
#ifdef RASPBERRY_PI
namespace raspberry = medicyc::cyclotroncontrolsystem::hardware::raspberry;
#endif

class Touch1Gui : public QWidget
{
    Q_OBJECT

public:
    explicit Touch1Gui();
    ~Touch1Gui();

public slots:

private slots:
    void ConnectNSingle(QString nsingle);
    void ConnectSourcePowerSupply(QString power_supply);
    void OpenEquipmentListWidget();
    void Ping();
    void DisplayKeyboard();
    void ShowEquipment(QString name);
    void ShowNSingle(QString name);
    void ShowSourcePowerSupply(QString name);
    bool UserQuestionPopup(QString question);
    void UserErrorPopup(QString message);
    void PlayBeep();

signals:
    void SIGNAL_Equipment_Selected(QString nsingle);
    // Specific signals for equipments
    void SIGNAL_NSingle_SetPointChanged(double value, bool polarity);
    void SIGNAL_NSingle_ValueChanged(double value, bool polarity);
    void SIGNAL_SourcePowerSupply_Voltage(double value);
    void SIGNAL_SourcePowerSupply_Current(double value);
    // Common signals for different equipments
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_IOLoad(double value);
    void SIGNAL_Error();
    void SIGNAL_Ok();
    void SIGNAL_On();
    void SIGNAL_Off();
    void SIGNAL_OnTarget();
    void SIGNAL_OffTarget();

    void SIGNAL_CommandStartup();
    void SIGNAL_CommandShutdown();
    void SIGNAL_CommandOn();
    void SIGNAL_CommandOff();
    void SIGNAL_CommandReset();
    void SIGNAL_CommandPolarityChange();
    void SIGNAL_CommandSetPoint(double value);
    void SIGNAL_CommandInc(int steps);
    void SIGNAL_CommandDec(int steps);

    void SIGNAL_Enable();
    void SIGNAL_Disable();

private:
    void FetchNSingles();
    void FetchSourcePowerSupplies();
    void CreateWidgets();
    QWidget* CreateNSingleWidget();
    QWidget* CreateSourcePowerSupplyWidget();

#ifdef RASPBERRY_PI
    void SetupGPIO();
    raspberry::GPIOHandler gpio_;
#endif
    QFrame* SetupTopFrame();
    QFrame* SetupQuickSelectionBar();
    QStringList GetQuickSelectionEquipments() const;

    QGridLayout* SetupNSingleValuesGrid();
    QGridLayout* SetupNSingleStatusGrid();
    QHBoxLayout* SetupNSingleCommandGrid();
    QGridLayout* SetupSourcePowerSupplyValuesGrid();
    QHBoxLayout* SetupSourcePowerSupplyCommandGrid();

    QStringList GetSubSystems() const;

    hw_nsingle::NSingleRepo nsingle_repo_;
    hw_source::SourcePowerSupplyRepo source_repo_;
    QSettings settings_;

    QStringList fast_access_equipments_;
    QString selected_equipment_ = "None";
    QTimer ping_timer_;

    QMap<QString, hw_nsingle::NSingleDbusAddress> nsingle_addresses_;
    QMap<QString, medicyc::cyclotron::NSingleInterface*> nsingle_interfaces_;
    QMap<QString, medicyc::cyclotron::NSingleControllerInterface*> nsingle_controller_interfaces_;

    QMap<QString, hw_source::PowerSupplyDbusAddress>  sourcepowersupply_addresses_;
    QMap<QString, hw_source::PowerSupplyConfig>  sourcepowersupply_configs_;
    QMap<QString, medicyc::cyclotron::SourcePowerSupplyInterface*> sourcepowersupply_interfaces_;

    std::vector<int> inc_steps_ = {1, 16, 60, 300}; // number of bits to increment with inc/dec commands
    int act_inc_steps_ = 1;

    QStackedLayout *stacked_layout_ = nullptr;
    QWidget *nsingle_widget_ = nullptr;
    QWidget *sourcepowersupply_widget_ = nullptr;
};

}

#endif
