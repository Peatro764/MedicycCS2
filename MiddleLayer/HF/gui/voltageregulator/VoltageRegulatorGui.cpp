#include "VoltageRegulatorGui.h"
#include "ui_VoltageRegulatorGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

VoltageRegulatorGui::VoltageRegulatorGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::VoltageRegulatorGui),
    niveau_hf1_(HFX::HF1),
    niveau_hf2_(HFX::HF2),
    dee_hf1_(HFX::HF1),
    dee_hf2_(HFX::HF2)
{
    qDebug() << "VoltageRegulatorGui::VoltageRegulatorGui";
    regulator_hf1_ = new VoltageRegulator(HFX::HF1, &dee_hf1_, &niveau_hf1_);
    regulator_hf2_ = new VoltageRegulator(HFX::HF2, &dee_hf2_, &niveau_hf2_);
    ui_->setupUi(this);
    ConnectSignals();
}

VoltageRegulatorGui::~VoltageRegulatorGui()
{
}

void VoltageRegulatorGui::ConnectSignals() {
    qDebug() << "VoltageRegulatorGui::ConnectSignals";
    // hf1
    QObject::connect(ui_->pushButton_max, &QPushButton::clicked, this, [&]() {
        regulator_hf1_->Configure_VoltageMax(ui_->spinBox_max->value());
    });
    QObject::connect(&niveau_hf1_, &Niveau::SIGNAL_Value, this, [&](double value) { ui_->label_niveau->setText(QString::number(value)); });
    QObject::connect(&dee_hf1_, &Dee::SIGNAL_DeeVoltage, this, [&](double value) { ui_->label_voltage->setText(QString::number(value)); });
    QObject::connect(ui_->pushButton_setlevel, &QPushButton::clicked, this, [&]() {
        regulator_hf1_->SetVoltage(ui_->lineEdit_setpoint->text().toDouble());
    });
    QObject::connect(ui_->pushButton_increment, &QPushButton::clicked, this, [&]() {
        regulator_hf1_->IncrementVoltage(ui_->lineEdit_delta->text().toDouble());
    });
    QObject::connect(ui_->pushButton_decrement, &QPushButton::clicked, this, [&]() {
        regulator_hf1_->DecrementVoltage(ui_->lineEdit_delta->text().toDouble());
    });
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, regulator_hf1_, &VoltageRegulator::Interrupt);
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, &niveau_hf1_, &Niveau::Interrupt);

    // hf2
    QObject::connect(ui_->pushButton_max_2, &QPushButton::clicked, this, [&]() {
        regulator_hf2_->Configure_VoltageMax(ui_->spinBox_max_2->value());
    });
    QObject::connect(&niveau_hf2_, &Niveau::SIGNAL_Value, this, [&](double value) { ui_->label_niveau_2->setText(QString::number(value)); });
    QObject::connect(&dee_hf2_, &Dee::SIGNAL_DeeVoltage, this, [&](double value) { ui_->label_voltage_2->setText(QString::number(value)); });
    QObject::connect(ui_->pushButton_setlevel_2, &QPushButton::clicked, this, [&]() {
        regulator_hf2_->SetVoltage(ui_->lineEdit_setpoint_2->text().toDouble());
    });
    QObject::connect(ui_->pushButton_increment_2, &QPushButton::clicked, this, [&]() {
        regulator_hf2_->IncrementVoltage(ui_->lineEdit_delta_2->text().toDouble());
    });
    QObject::connect(ui_->pushButton_decrement_2, &QPushButton::clicked, this, [&]() {
        regulator_hf2_->DecrementVoltage(ui_->lineEdit_delta_2->text().toDouble());
    });
    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, regulator_hf2_, &VoltageRegulator::Interrupt);
    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::Interrupt);
}

}
