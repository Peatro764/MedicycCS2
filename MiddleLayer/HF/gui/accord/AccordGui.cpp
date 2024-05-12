#include "AccordGui.h"
#include "ui_AccordGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

AccordGui::AccordGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AccordGui),
    membrane_hf1_(HFX::HF1),
    membrane_hf2_(HFX::HF2),
    niveau_hf1_(HFX::HF1),
    niveau_hf2_(HFX::HF2),
    dee_hf1_(HFX::HF1),
    dee_hf2_(HFX::HF2)
{
    qDebug() << "AccordGui::AccordGui";
    accord_hf1_ = new Accord(HFX::HF1, &membrane_hf1_, &dee_hf1_, &niveau_hf1_);
    accord_hf2_ = new Accord(HFX::HF2, &membrane_hf2_, &dee_hf2_, &niveau_hf2_);
    ui_->setupUi(this);
    ConnectSignals();
}

AccordGui::~AccordGui()
{
}

void AccordGui::ConnectSignals() {
    qDebug() << "AccordGui::ConnectSignals";
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, [&]() { accord_hf1_->Interrupt(); });
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, [&]() { membrane_hf1_.Interrupt(); });
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, [&]() { niveau_hf1_.Interrupt(); });

    QObject::connect(ui_->pushButton_find_accord, &QPushButton::clicked, [&]() { accord_hf1_->FindAccord(); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_Accorded, this, [&]() { ui_->radiobutton_accorded->setChecked(true); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_NotAccorded, this, [&]() { ui_->radiobutton_accorded->setChecked(false); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_VoltageAboveLowerLimit, this, [&]() { qDebug() << "AboveLower"; ui_->radiobutton_above_lower->setChecked(true); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_VoltageBelowLowerLimit, this, [&]() { qDebug() << "BelowLower";ui_->radiobutton_above_lower->setChecked(false); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_VoltageAboveUpperLimit, this, [&]() { qDebug() << "AboveUpper";ui_->radiobutton_above_upper->setChecked(true); });
    QObject::connect(accord_hf1_, &Accord::SIGNAL_VoltageBelowUpperLimit, this, [&]() { qDebug() << "BelowUpper";ui_->radiobutton_above_upper->setChecked(false); });

    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, [&]() { accord_hf2_->Interrupt(); });
    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, [&]() { membrane_hf2_.Interrupt(); });
    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, [&]() { niveau_hf2_.Interrupt(); });
    QObject::connect(ui_->pushButton_find_accord_2, &QPushButton::clicked, [&]() { accord_hf2_->FindAccord(); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_Accorded, this, [&]() { ui_->radiobutton_accorded_2->setChecked(true); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_NotAccorded, this, [&]() { ui_->radiobutton_accorded_2->setChecked(false); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_VoltageAboveLowerLimit, this, [&]() { ui_->radiobutton_above_lower_2->setChecked(true); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_VoltageBelowLowerLimit, this, [&]() { ui_->radiobutton_above_lower_2->setChecked(false); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_VoltageAboveUpperLimit, this, [&]() { ui_->radiobutton_above_upper_2->setChecked(true); });
    QObject::connect(accord_hf2_, &Accord::SIGNAL_VoltageBelowUpperLimit, this, [&]() { ui_->radiobutton_above_upper_2->setChecked(false); });
}

}
