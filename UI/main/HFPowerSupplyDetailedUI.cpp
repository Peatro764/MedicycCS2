#include "HFPowerSupplyDetailedUI.h"

#include <QVBoxLayout>

#include "Utils.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

HFPowerSupplyDetailedUI::HFPowerSupplyDetailedUI(QWidget *parent) :
    QWidget(parent),
    powersupplies_interface_(medicyc::cyclotron::HFPowerSuppliesInterface(QString("medicyc.cyclotron.hardware.hfpowersupplies"), QString("/PowerSupplies"), medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    if (!powersupplies_interface_.isValid()) {
        qDebug() << "DBUS connection error";
    }

    CreateDevices();

    // Central rows (valve indicators)
    QGridLayout* grid_layout = SetupStatusIcons();

    // Bottom row
    auto pushButton_close =  shared::utils::GetPushButton("Quitter", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_close, &QPushButton::clicked, this, &HFPowerSupplyDetailedUI::close);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(grid_layout);
    v_layout->addWidget(pushButton_close);

    QObject::connect(&powersupplies_interface_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Device_State, this, &HFPowerSupplyDetailedUI::UpdateDeviceState);
    QObject::connect(&powersupplies_interface_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Device_Defaut, this, &HFPowerSupplyDetailedUI::UpdateDeviceDefaut);
    QObject::connect(&powersupplies_interface_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Device_Voltage, this, &HFPowerSupplyDetailedUI::UpdateDeviceVoltage);
}

HFPowerSupplyDetailedUI::~HFPowerSupplyDetailedUI() {
    qDebug() << "HFPowerSupplyDetailedUI::~HFPowerSupplyDetailedUI";
    for (const auto& [name, widget] : device_states_) {
        delete widget;
    }
    for (const auto& [name, widget] : device_defauts_) {
        delete widget;
    }
    for (const auto& [name, widget] : device_voltages_) {
        delete widget;
    }
    device_states_.clear();
    device_defauts_.clear();
    device_voltages_.clear();
}

void HFPowerSupplyDetailedUI::ReadDeviceStatus() {
    powersupplies_interface_.ReadBottomLevelDeviceStatus();
}

QGridLayout* HFPowerSupplyDetailedUI::SetupStatusIcons() {
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(shared::utils::GetLabel("Nom", shared::FONT::SMALL_LABEL), 0, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("On", shared::FONT::SMALL_LABEL), 0, 1, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Défaut", shared::FONT::SMALL_LABEL), 0, 2, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("V", shared::FONT::SMALL_LABEL), 0, 3, Qt::AlignHCenter);

    int row = 1;
    for (auto& d : devices_) {
        gridLayout->addWidget(shared::utils::GetLabel(d, shared::FONT::SMALL_LABEL), row, 0, Qt::AlignLeft);
        gridLayout->addWidget(device_states_[d], row, 1, Qt::AlignHCenter);
        gridLayout->addWidget(device_defauts_[d], row, 2, Qt::AlignHCenter);
        gridLayout->addWidget(device_voltages_[d], row, 3, Qt::AlignHCenter);
        row++;
    }
    return gridLayout;
}

void HFPowerSupplyDetailedUI::CreateDevices() {
    try {
        for (auto &c: devices_) {
            qDebug() << "HFPowerSupplyDetailedUI::CreateDevices " << c;
            device_states_[c] = new GenericDisplayButton(nullptr, 20);
            device_states_[c]->SetupAsColorChanging(Qt::green, Qt::red);
            device_defauts_[c] = new GenericDisplayButton(nullptr, 20);
            device_defauts_[c]->SetupAsColorChanging(Qt::green, Qt::red);
            device_voltages_[c] = new QLabel("-");
        }
    } catch (std::exception& exc) {
        qWarning() << "HFPowerSupplyDetailedUI::CreateDevices Caught exception " << exc.what();
    }
}

void HFPowerSupplyDetailedUI::UpdateDeviceState(const QString& channel, bool state) {
    if (auto search = device_states_.find(channel); search != device_states_.end()) {
        state ? device_states_[channel]->On() : device_states_[channel]->Off();
    } else {
        qWarning() << "HFPowerSupplyDetailedUI::UpdateDeviceState Unknown channel " << channel << " state " << state;
    }
}

void HFPowerSupplyDetailedUI::UpdateDeviceDefaut(const QString& channel, bool active) {
    if (auto search = device_defauts_.find(channel); search != device_defauts_.end()) {
        !active ? device_defauts_[channel]->On() : device_defauts_[channel]->Off();
    } else {
        qWarning() << "HFPowerSupplyDetailedUI::UpdateDeviceDefaut Unknown channel " << channel << " state " << active;
    }
}

void HFPowerSupplyDetailedUI::UpdateDeviceVoltage(const QString& channel, double value) {
    if (auto search = device_voltages_.find(channel); search != device_voltages_.end()) {
        device_voltages_[channel]->setText(QString::number(value));
    } else {
        qWarning() << "HFPowerSupplyDetailedUI::UpdateDeviceVoltage Unknown channel " << channel << " value " << value;
    }
}

}
