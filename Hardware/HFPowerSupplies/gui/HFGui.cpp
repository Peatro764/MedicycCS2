#include "HFGui.h"
#include "ui_HFGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

HFGui::HFGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::HFGui)
{
    qDebug() << "HFGui::HFGui";
    ui_->setupUi(this);
    ConnectSignals();
}

HFGui::~HFGui()
{
}

void HFGui::ConnectSignals() {
    qDebug() << "HFGui::ConnectSignals";
    QObject::connect(ui_->pushButton_powersupply_preamplis_on, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::PreAmplisOn);
    QObject::connect(ui_->pushButton_powersupply_preamplis_off, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::PreAmplisOff);
    QObject::connect(ui_->pushButton_powersupply_amplis_on, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::AmplisOn);
    QObject::connect(ui_->pushButton_powersupply_amplis_off, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::AmplisOff);

    QObject::connect(ui_->pushButton_powersupply_interrupt, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::Interrupt);
    QObject::connect(ui_->pushButton_powersupply_startup, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::Startup);
    QObject::connect(ui_->pushButton_powersupply_shutdown, &QPushButton::clicked, &power_supplies_, &HFPowerSupplies::Shutdown);
}

}
