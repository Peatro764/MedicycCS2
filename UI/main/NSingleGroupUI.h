#ifndef NSINGLEGROUPUI_H
#define NSINGLEGROUPUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>

#include "ISubSystemUI.h"
#include "nsinglecontroller_interface.h"
#include "nsinglegroup_interface.h"
#include "Style.h"
#include "Enumerations.h"
#include "NSingleRepo.h"
#include "NSingleDbusAddress.h"
#include "EquipmentGroupValues.h"

class QWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class GenericDisplayButton;
class PoleLevelIndicator;

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;
namespace middlelayer = medicyc::cyclotron::middlelayer;

class NSingleUIElements {
public:
    NSingleUIElements() {}
    ~NSingleUIElements() {}
    bool available;
    QPushButton* startup;
    QPushButton* shutdown;
    GenericDisplayButton* connected;
    PoleLevelIndicator* io_load;
    GenericDisplayButton* switched_on;
    GenericDisplayButton* state;
    GenericDisplayButton* ontarget;
    QLabel *name;
    QLabel *config;
    QLabel *commanded_value;
    QLabel *actual_value;
};

class NSingleGroupUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    NSingleGroupUI(QWidget *parent, QString title, medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem sub_system);
    ~NSingleGroupUI();
    EquipmentGroupValues GetGroupValues() const;

public slots:
    void Configure(QString name) override;
    void Startup() override;
    void Shutdown() override;
    void Interrupt() override;

signals:
    void SIGNAL_State_Off() override;
    void SIGNAL_State_Intermediate() override;
    void SIGNAL_State_Ready() override;
    void SIGNAL_State_Unknown() override;
    void SIGNAL_StartupFinished() override;
    void SIGNAL_ShutdownFinished() override;

private slots:
    void Ping();
    void ShowNSingle(QString name, bool flag);
    void ShowNSingle(medicyc::cyclotroncontrolsystem::ui::main::NSingleUIElements *e, bool flag);
    void StartupAfterConfirmation();
    void ShutdownAfterConfirmation();

private:
    QGridLayout* SetupGridLayout();
    NSingleUIElements* CreateNSingleUIElements(QString short_name, bool available);
    void ConnectNSingleUIElements(NSingleUIElements* uielements, medicyc::cyclotron::NSingleControllerInterface* controller);
    void PlaceOnGridLayout(NSingleUIElements* elements);
    void AddSubSystemNSingles(QString sub_system);
    void AddNSingle(QString full_name, QString short_name, hw_nsingle::NSingleDbusAddress nsingle_address, bool available);

    QString title_;
    medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem sub_system_;
    QString sub_system_string_;

    QGridLayout *grid_layout_;
    middlelayer::NSingleGroupInterface group_interface_;
    QMap<QString, NSingleUIElements*> nsingle_uielements_;
    QMap<QString, medicyc::cyclotron::NSingleControllerInterface*> nsingle_controllers_;
    int gridlayout_active_row_ = 0;
    QTimer ping_timer_;
    hw_nsingle::NSingleRepo repo_;
};

}

#endif // NSINGLEGROUPUI_H
