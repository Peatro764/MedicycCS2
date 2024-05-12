#include "DeeVoltageGui.h"
#include "ui_DeeVoltageGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

DeeVoltageGui::DeeVoltageGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::DeeVoltageGui),
    dee_hf1_(HFX::HF1),
    dee_hf2_(HFX::HF2)
{
    qDebug() << "DeeVoltageGui::DeeVoltageGui";
    ui_->setupUi(this);
    ConnectSignals();
}

DeeVoltageGui::~DeeVoltageGui()
{
}

void DeeVoltageGui::ConnectSignals() {
    qDebug() << "DeeVoltageGui::ConnectSignals";

    QObject::connect(ui_->pushButton_regulation_on, &QPushButton::clicked, &dee_hf1_, &Dee::SetRegulationOn);
    QObject::connect(ui_->pushButton_regulation_off, &QPushButton::clicked, &dee_hf1_, &Dee::SetRegulationOff);
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_RegulationOn, this, [&]() { ui_->radioButton_regulation_on->setChecked(true); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_RegulationOff, this, [&]() { ui_->radioButton_regulation_on->setChecked(false); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_DeeVoltage , this, [&](double kv) { ui_->label_voltage->setText(QString::number(kv)); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_DeePhase , this, [&](double value) { ui_->label_phase->setText(QString::number(value)); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_VoltageIncreasing, this, [&]() { ui_->radioButton_increasing->setChecked(true); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_VoltageDecreasing, this, [&]() { ui_->radioButton_increasing->setChecked(false); });

    QObject::connect(ui_->pushButton_regulation_on_2, &QPushButton::clicked, &dee_hf2_, &Dee::SetRegulationOn);
    QObject::connect(ui_->pushButton_regulation_off_2, &QPushButton::clicked, &dee_hf2_, &Dee::SetRegulationOff);
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_RegulationOn, this, [&]() { ui_->radioButton_regulation_on_2->setChecked(true); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_RegulationOff, this, [&]() { ui_->radioButton_regulation_on_2->setChecked(false); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_DeeVoltage , this, [&](double kv) { ui_->label_voltage_2->setText(QString::number(kv)); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_DeePhase , this, [&](double value) { ui_->label_phase_2->setText(QString::number(value)); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_VoltageIncreasing, this, [&]() { ui_->radioButton_increasing_2->setChecked(true); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_VoltageDecreasing, this, [&]() { ui_->radioButton_increasing_2->setChecked(false); });

}

}
