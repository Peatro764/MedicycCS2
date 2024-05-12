#include "ControllerGui.h"
#include "ui_ControllerGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

ControllerGui::ControllerGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::ControllerGui)
{
    qDebug() << "ControllerGui::ControllerGui";
    ui_->setupUi(this);
    ConnectSignals();
}

ControllerGui::~ControllerGui()
{
}

void ControllerGui::ConnectSignals() {
    qDebug() << "ControllerGui::ConnectSignals";

    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, this, [&, this]() { controller_.Interrupt(); });
    QObject::connect(ui_->pushButton_startup, &QPushButton::clicked, this, [&, this]() { controller_.Startup(); });
    QObject::connect(ui_->pushButton_shutdown, &QPushButton::clicked, this, [&, this]() { controller_.Shutdown(); });
    QObject::connect(ui_->pushButton_findaccord, &QPushButton::clicked, this, [&, this]() { controller_.FindAccord(); });

    QObject::connect(&controller_, &HFController::SIGNAL_Dee_Voltage, this, [&](int hfx, double value) {
        if (hfx == static_cast<int>(HFX::HF1)) ui_->label_dee_voltage_1->setText(QString::number(value));
        else ui_->label_dee_voltage_2->setText(QString::number(value));
    });
    QObject::connect(&controller_, &HFController::SIGNAL_Niveau_Value, this, [&](int hfx, double value) {
        if (hfx == static_cast<int>(HFX::HF1)) ui_->label_niveau1->setText(QString::number(value));
        else ui_->label_niveau2->setText(QString::number(value));
    });
    QObject::connect(&controller_, &HFController::SIGNAL_Niveau_State, this, [&](int hfx, bool state) {
        if (hfx == static_cast<int>(HFX::HF1)) ui_->radioButton_niveau1->setChecked(state);
        else ui_->radioButton_niveau2->setChecked(state);
    });
    QObject::connect(ui_->pushButton_niveau1_inc, &QPushButton::clicked, this, [&, this]() { controller_.IncNiveau(static_cast<int>(HFX::HF1)); });
    QObject::connect(ui_->pushButton_niveau2_inc, &QPushButton::clicked, this, [&, this]() { controller_.IncNiveau(static_cast<int>(HFX::HF2)); });
    QObject::connect(ui_->pushButton_niveau1_dec, &QPushButton::clicked, this, [&, this]() { controller_.DecNiveau(static_cast<int>(HFX::HF1)); });
    QObject::connect(ui_->pushButton_niveau2_dec, &QPushButton::clicked, this, [&, this]() { controller_.DecNiveau(static_cast<int>(HFX::HF2)); });
}

}
