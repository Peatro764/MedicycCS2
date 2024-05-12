#include "SourceUI.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QSound>
#include <QMessageBox>

#include "GenericDisplayButton.h"
#include "PoleLevelIndicator.h"
#include "adc_interface.h"
#include "source_interface.h"
#include "Utils.h"
#include "DBus.h"
#include "PowerSupplyDbusAddress.h"
#include "Enumerations.h"

#include "Style.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

SourceUI::SourceUI(QWidget *parent) :
    QWidget(parent),
    source_repo_("_prod"),
    nsingle_repo_("_prod"),
    source_interface_(middlelayer::SourceInterface(QString("medicyc.cyclotron.middlelayer.source"), QString("/Source"), medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    // Relay group state to main gui
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_StartupFinished, this, &SourceUI::SIGNAL_StartupFinished);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_ShutdownFinished, this, &SourceUI::SIGNAL_ShutdownFinished);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_State_Off, this, &SourceUI::SIGNAL_State_Off);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_State_Intermediate, this, &SourceUI::SIGNAL_State_Intermediate);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_State_Ready, this, &SourceUI::SIGNAL_State_Ready);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_State_Unknown, this, &SourceUI::SIGNAL_State_Unknown);

    CreatePowerSupplies();
    SetupInjectionCurrentPolling();
    SetupUI();
    source_interface_.QueryConfiguration();
    qDebug() << "SOURCE " << source_interface_.isValid();
}

SourceUI::~SourceUI() {
    for (auto& k : powersupply_uielements_.values()) delete k;
    for (auto& k : sourcepowersupply_interfaces_.values()) delete k;
    for (auto& k : nsingle_interfaces_.values()) delete k;
    powersupply_primary_param_.clear();
    powersupply_uielements_.clear();
    sourcepowersupply_configs_.clear();
    sourcepowersupply_interfaces_.clear();
    nsingle_interfaces_.clear();
}

void SourceUI::StartupAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Démarrer la source ?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { source_interface_.Startup(); }
}

void SourceUI::ShutdownAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Arrêter la source ?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { source_interface_.Shutdown(); }
}

void SourceUI::Startup() {
   source_interface_.Startup();
}

void SourceUI::Shutdown() {
   source_interface_.Shutdown();
}

void SourceUI::Interrupt() {
    source_interface_.Interrupt();
}

void SourceUI::Configure(QString name) {
    qDebug() << "SourceUI::Configure " << name;
    source_interface_.Configure(name);
}

void SourceUI::SetupUI() {
    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("Source H-", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel *l_config = shared::utils::GetLabel("-", shared::FONT::LARGE_LABEL);
    l_config->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_config, Qt::AlignRight | Qt::AlignVCenter);
    QObject::connect(&source_interface_, &middlelayer::SourceInterface::SIGNAL_Configuration, this, [&, l_config](QString name) { l_config->setText(name); });

    // Line separator
    auto line_1 = new QFrame;
    line_1->setFixedHeight(3);
    line_1->setFrameShadow(QFrame::Sunken);
    line_1->setLineWidth(2);
    line_1->setStyleSheet("background-color: cornflowerblue");

    // Central rows (powersupply info)
    QGridLayout* grid_layout = new QGridLayout;
    grid_layout->setSpacing(10);
    grid_layout->addLayout(CreateSourcePowerSupplyLayout("Filament"), 0, 0);
    grid_layout->addLayout(CreateSourcePowerSupplyLayout("Arc"), 0, 1);
    grid_layout->addLayout(CreateSourcePowerSupplyLayout("Extraction"), 1, 0);
    grid_layout->addLayout(CreateNSinglePowerSupplyLayout("Injection_33kV"), 1, 1);

    // Bottom rows (control pushbuttons)
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    auto pushButton_startup =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_shutdown =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_interrupt =  shared::utils::GetPushButton("Interrompre", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_startup, &QPushButton::clicked, this, &SourceUI::StartupAfterConfirmation);
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, &SourceUI::ShutdownAfterConfirmation);
    QObject::connect(pushButton_interrupt, &QPushButton::clicked, this, &SourceUI::Interrupt);
    bottom_layout->addWidget(pushButton_startup);
    bottom_layout->addWidget(pushButton_shutdown);
    bottom_layout->addWidget(pushButton_interrupt);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout);
    v_layout->addSpacing(20);
    v_layout->addLayout(bottom_layout);
}

void SourceUI::CreatePowerSupplies() {
    try {
        auto SetupSourcePowerSupply = [&] (QString name, QString pri_param) {
            powersupply_primary_param_[name] = pri_param;
            auto dbus_address = source_repo_.GetDbusAddress(name);
            sourcepowersupply_configs_[name] = source_repo_.GetConfig(name);
            sourcepowersupply_interfaces_[name] = new medicyc::cyclotron::SourcePowerSupplyInterface(
                        dbus_address.interface_address(), dbus_address.object_name(),
                        medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            qDebug() << QString("SourceUI::CreatePowerSupplies DBUS connection ") + name + " " + (sourcepowersupply_interfaces_[name]->isValid() ? "OK" : "ERROR");
            powersupply_uielements_[name] = CreateSourcePowerSupplyUIElements(name);
            ConnectSourcePowerSupplyUIElements(name);
        };

        // Primary and secondary parameter hardcoded
        auto SetupNSinglePowerSupply = [&](QString name, QString pri_param) {
            powersupply_primary_param_[name] = pri_param;
            auto dbus_address = nsingle_repo_.GetNSingleDbusAddress(name);
            nsingle_interfaces_[name] = new medicyc::cyclotron::NSingleControllerInterface(dbus_address.interface_address(), dbus_address.controller_object_name(),
                                                                                                                            medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            qDebug() << QString("SourceUI::CreatePowerSupplies DBUS connection Injection33kV ") + (nsingle_interfaces_[name]->isValid() ? "OK" : "ERROR");
            powersupply_uielements_[name] = CreateNSinglePowerSupplyUIElements(name);
            ConnectNSinglePowerSupplyUIElements(name);
        };

        // Primary parameter ("V" or "I") is shown in large centered, secondary (=!primary) small at top left corner. TODO: move to db
        SetupSourcePowerSupply("Extraction", "V");
        SetupSourcePowerSupply("Filament", "I");
        SetupSourcePowerSupply("Arc", "I");
        SetupNSinglePowerSupply("Injection_33kV", "V");
    } catch(std::exception& exc) {
        qWarning() << "SourceUI::CreatePowerSupplies Caught exception, bailing out: " << exc.what();
        exit(-1);
    }
}

PowerSupplyUIElements* SourceUI::CreateSourcePowerSupplyUIElements(QString name) {
    assert(powersupply_primary_param_.contains(name));
    qDebug() << "SourceUI::CreateSourcePowerSupplyUIElements " << name;
    PowerSupplyUIElements* ui_element = new PowerSupplyUIElements;
    ui_element->startup  = shared::utils::GetPushButton("D", shared::BUTTON::SMALL_HEIGHT);
    ui_element->shutdown = shared::utils::GetPushButton("A", shared::BUTTON::SMALL_HEIGHT);
    ui_element->startup->setMaximumWidth(35);
    ui_element->shutdown->setMaximumWidth(35);
    ui_element->startup->setMinimumWidth(35);
    ui_element->shutdown->setMinimumWidth(35);
    ui_element->startup->setMaximumHeight(30);
    ui_element->startup->setMinimumHeight(30);
    ui_element->shutdown->setMaximumHeight(30);
    ui_element->shutdown->setMinimumHeight(30);
    ui_element->name = shared::utils::GetLabel(name, shared::FONT::LARGE_LABEL);
    if (powersupply_primary_param_[name] == QString("I")) {
        ui_element->act_current = shared::utils::GetLabel("NA", shared::FONT::XXXLARGE_LABEL);
        ui_element->act_current->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui_element->act_current->setAlignment(Qt::AlignCenter);
        ui_element->act_voltage = shared::utils::GetLabel("NA", shared::FONT::LARGE_LABEL);
    } else {
        ui_element->act_voltage = shared::utils::GetLabel("NA", shared::FONT::XXXLARGE_LABEL);
        ui_element->act_voltage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui_element->act_voltage->setAlignment(Qt::AlignCenter);
        ui_element->act_current = shared::utils::GetLabel("NA", shared::FONT::LARGE_LABEL);
    }
    ui_element->switched_on = new GenericDisplayButton(this, 30);
    ui_element->switched_on->SetupAsColorChanging(Qt::green, Qt::red);
    return ui_element;
}

void SourceUI::ConnectSourcePowerSupplyUIElements(QString name) {
    assert(sourcepowersupply_interfaces_.contains(name) && sourcepowersupply_configs_.contains(name) && powersupply_uielements_.contains(name));
    auto iface = sourcepowersupply_interfaces_[name];
    auto config = sourcepowersupply_configs_[name];
    auto ui = powersupply_uielements_[name];
    QObject::connect(ui->startup, &QPushButton::clicked, iface, &medicyc::cyclotron::SourcePowerSupplyInterface::Startup);
    QObject::connect(ui->shutdown, &QPushButton::clicked, iface, &medicyc::cyclotron::SourcePowerSupplyInterface::Shutdown);
    QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, ui->switched_on, &GenericDisplayButton::Off);
    QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_On, ui->switched_on, &GenericDisplayButton::On);
    QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Voltage,
                     this, [ui, config](double value) { ui->act_voltage->setText(QString::number(value, 'f', 2) + " " + config.voltage_unit()); });
    QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Current,
                     this, [ui, config](double value) { ui->act_current->setText(QString::number(value, 'f', 2) + " " + config.current_unit()); });
}

QVBoxLayout* SourceUI::CreateSourcePowerSupplyLayout(QString name) {
    assert(powersupply_primary_param_.contains(name));
    assert(powersupply_uielements_.contains(name));
    auto elements = powersupply_uielements_[name];

    // Global layout
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *top_layout = new QHBoxLayout;
    top_layout->setSpacing(3);
    top_layout->addWidget(elements->name, Qt::AlignLeft);

    // Values frame
    QFrame *values_frame = new QFrame;
    values_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    values_frame->setObjectName("values_frame_" + elements->name->text());
    values_frame->setStyleSheet("QFrame#" + values_frame->objectName() + "{ background: transparent; border: 0.2ex solid white; border-radius: 3px;}");
    QVBoxLayout* values_frame_layout = new QVBoxLayout;
    values_frame->setLayout(values_frame_layout);
    if (powersupply_primary_param_[name] == QString("I")) {
        values_frame_layout->addWidget(elements->act_voltage, Qt::AlignTop | Qt::AlignLeft);
        values_frame_layout->addWidget(elements->act_current, Qt::AlignHCenter);
        elements->act_current->setMinimumHeight(100);
    } else {
        values_frame_layout->addWidget(elements->act_current, Qt::AlignTop | Qt::AlignLeft);
        values_frame_layout->addWidget(elements->act_voltage, Qt::AlignHCenter);
        elements->act_voltage->setMinimumHeight(100);
    }
    values_frame_layout->addSpacing(40);

    // Status layout
    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->setSpacing(3);
    QFont header_font = shared::FONT::SMALL_HEADER;
    grid_layout->addWidget(shared::utils::GetLabel("Allumée", header_font), 0, 2, Qt::AlignHCenter);

    grid_layout->addWidget(elements->startup, 1, 0, Qt::AlignHCenter);
    grid_layout->addWidget(elements->shutdown, 1, 1, Qt::AlignHCenter);
    grid_layout->addWidget(elements->switched_on, 1, 2, Qt::AlignHCenter);

    layout->addLayout(top_layout);
    layout->addWidget(values_frame);
    layout->addLayout(grid_layout);

    return layout;
}

PowerSupplyUIElements* SourceUI::CreateNSinglePowerSupplyUIElements(QString name) {
    qDebug() << "SourceUI::CreateNSinglePowerSupplyUIElements " << name;
    PowerSupplyUIElements* ui_element = new PowerSupplyUIElements;
    ui_element->startup  = shared::utils::GetPushButton("D", shared::BUTTON::SMALL_HEIGHT);
    ui_element->shutdown = shared::utils::GetPushButton("A", shared::BUTTON::SMALL_HEIGHT);
    ui_element->startup->setMaximumWidth(35);
    ui_element->shutdown->setMaximumWidth(35);
    ui_element->startup->setMinimumWidth(35);
    ui_element->shutdown->setMinimumWidth(35);
    ui_element->startup->setMaximumHeight(30);
    ui_element->startup->setMinimumHeight(30);
    ui_element->shutdown->setMaximumHeight(30);
    ui_element->shutdown->setMinimumHeight(30);
    ui_element->name = shared::utils::GetLabel(name, shared::FONT::LARGE_LABEL);
    ui_element->act_voltage = shared::utils::GetLabel("NA", shared::FONT::XXXLARGE_LABEL);
    ui_element->act_voltage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui_element->act_voltage->setAlignment(Qt::AlignCenter);
    ui_element->act_current = shared::utils::GetLabel("NA", shared::FONT::LARGE_LABEL);
    ui_element->connected = new GenericDisplayButton(this, 30);
    ui_element->connected->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->io_load = new PoleLevelIndicator(this, 10, 30, 3, 8);
    ui_element->state = new GenericDisplayButton(this, 30);
    ui_element->state->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->switched_on = new GenericDisplayButton(this, 30);
    ui_element->switched_on->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->ontarget = new GenericDisplayButton(this, 30);
    ui_element->ontarget->SetupAsCenterFilling(QRgb(0x209fdf));
    return ui_element;
}

void SourceUI::ConnectNSinglePowerSupplyUIElements(QString name) {
    assert(nsingle_interfaces_.contains(name) && powersupply_uielements_.contains(name));
    auto iface = nsingle_interfaces_[name];
    auto ui = powersupply_uielements_[name];

    QObject::connect(ui->startup, &QPushButton::clicked, iface, &medicyc::cyclotron::NSingleControllerInterface::Startup);
    QObject::connect(ui->shutdown, &QPushButton::clicked, iface, &medicyc::cyclotron::NSingleControllerInterface::Shutdown);

    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_IOLoad , this, [ui] (double value) { ui->io_load->SetLevel(value); });
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Disconnected, this, [ui] () { ui->connected->Off(); });
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Connected, this, [ui]() { ui->connected->On();});
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, ui->switched_on, &GenericDisplayButton::Off);
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, ui->switched_on, &GenericDisplayButton::On);
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, ui->ontarget, &GenericDisplayButton::On);
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, ui->ontarget, &GenericDisplayButton::Off);
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [ui](double value, bool polarity) {
        ui->act_voltage->setText((polarity ? "+" : "-") + QString::number(value / 1000, 'f', 2) + " kV"); // TODO: add unit in nsingle config + deal with V to kV conversion
    });
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DesValue, this, [ui](double value, bool polarity) {
        ui->des_voltage = (polarity ? "+" : "-") + QString::number(value / 1000, 'f', 2) + " kV"; // TODO: add unit in nsingle config + deal with V to kV conversion
    });
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareError, this, [ui]() { ui->state->Off();});
    QObject::connect(iface, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareOk, this, [ui]() { ui->state->On(); });
    QObject::connect(this, &SourceUI::SIGNAL_InjectionCurrent, this, [ui](double current) {
        ui->act_current->setText(QString::number(current, 'f', 2) + " mA"); }); // TODO: use unit from adc channel
    iface->QueryState();
}

QVBoxLayout* SourceUI::CreateNSinglePowerSupplyLayout(QString name) {
    assert(powersupply_uielements_.contains(name));
    auto elements = powersupply_uielements_[name];

    // Global layout
    QVBoxLayout *layout = new QVBoxLayout;

    QHBoxLayout *top_layout = new QHBoxLayout;
    top_layout->setSpacing(3);
    top_layout->addWidget(elements->name, Qt::AlignLeft);

    // Values frame
    QFrame *values_frame = new QFrame;
    values_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    values_frame->setObjectName("values_frame_" + elements->name->text());
    values_frame->setStyleSheet("QFrame#" + values_frame->objectName() + "{ background: transparent; border: 0.2ex solid white; border-radius: 3px;}");
    QVBoxLayout* values_frame_layout = new QVBoxLayout;
    values_frame->setLayout(values_frame_layout);
    values_frame_layout->addWidget(elements->act_current, Qt::AlignTop | Qt::AlignLeft);
    values_frame_layout->addWidget(elements->act_voltage, Qt::AlignHCenter);
    values_frame_layout->addSpacing(40);
    elements->act_voltage->setMinimumHeight(100);

    // Status layout
    QGridLayout *grid_layout = new QGridLayout;
    grid_layout->setSpacing(3);
    QFont header_font = shared::FONT::SMALL_HEADER;
    grid_layout->addWidget(shared::utils::GetLabel("Cxn", header_font), 0, 2, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Com", header_font), 0, 3, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Défaut", header_font), 0, 4, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Allumée", header_font), 0, 5, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Ontarget", header_font), 0, 6, Qt::AlignHCenter);

    grid_layout->addWidget(elements->startup, 1, 0, Qt::AlignHCenter);
    grid_layout->addWidget(elements->shutdown, 1, 1, Qt::AlignHCenter);
    grid_layout->addWidget(elements->connected, 1, 2, Qt::AlignHCenter);
    grid_layout->addWidget(elements->io_load, 1, 3, Qt::AlignHCenter);
    grid_layout->addWidget(elements->state, 1, 4, Qt::AlignHCenter);
    grid_layout->addWidget(elements->switched_on, 1, 5, Qt::AlignHCenter);
    grid_layout->addWidget(elements->ontarget, 1, 6, Qt::AlignHCenter);

    layout->addLayout(top_layout);
    layout->addWidget(values_frame);
    layout->addLayout(grid_layout);

    return layout;
}

void SourceUI::SetupInjectionCurrentPolling() {
    medicyc::cyclotron::ADCInterface* adc = new medicyc::cyclotron::ADCInterface("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
    QTimer *tInjectionCurrent = new QTimer;
    tInjectionCurrent->setSingleShot(false);
    tInjectionCurrent->setInterval(1000);
    tInjectionCurrent->start();
    QObject::connect(tInjectionCurrent, &QTimer::timeout, this, [&, adc]() { adc->Read("Courant Injection 33 kV"); });
    QObject::connect(adc, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, [&](const QString& channel, double value, QString unit) {
        (void)unit;
        if (channel == QString("Courant Injection 33 kV")) {
            emit SIGNAL_InjectionCurrent(value);
        }
    });
}

EquipmentGroupValues SourceUI::GetGroupValues() const {
    EquipmentGroupValues group_values(medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystemToString(global::Enumerations::SubSystem::SOURCE));
    for (auto k : powersupply_uielements_.keys()) {
        PowerSupplyUIElements *e = powersupply_uielements_.value(k);
        EquipmentValues current_value(e->switched_on->text(), e->des_current, e->act_current->text());
        group_values.AddEquipment(k + " Courant", current_value);
        EquipmentValues voltage_value(e->switched_on->text(), e->des_voltage, e->act_voltage->text());
        group_values.AddEquipment(k + " Tension", voltage_value);
    }
    return group_values;
}


} // ns

