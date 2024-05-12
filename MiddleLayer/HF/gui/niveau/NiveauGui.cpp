#include "NiveauGui.h"
#include "ui_NiveauGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

NiveauGui::NiveauGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::NiveauGui),
    niveau_hf1_(HFX::HF1),
    niveau_hf2_(HFX::HF2)
{
    qDebug() << "NiveauGui::NiveauGui";
    ui_->setupUi(this);
    ConnectSignals();
}

NiveauGui::~NiveauGui()
{
}

void NiveauGui::ConnectSignals() {
    qDebug() << "NiveauGui::ConnectSignals";
    QObject::connect(ui_->pushButton_interrupt, &QPushButton::clicked, [&]() { niveau_hf1_.Interrupt(); });
    QObject::connect(ui_->pushButton_setlevel, &QPushButton::clicked, [&]() { niveau_hf1_.SetLevel(ui_->doubleSpinBox_value->value()); });
    QObject::connect(ui_->pushButton_niveau_on, &QPushButton::clicked, &niveau_hf1_, &Niveau::On);
    QObject::connect(ui_->pushButton_niveau_off, &QPushButton::clicked, &niveau_hf1_, &Niveau::Off);
    QObject::connect(ui_->pushButton_niveau_inc, &QPushButton::clicked, &niveau_hf1_, &Niveau::Increment);
    QObject::connect(ui_->pushButton_niveau_dec, &QPushButton::clicked, &niveau_hf1_, &Niveau::Decrement);
    QObject::connect(ui_->pushButton_setstartupfinalvalue, &QPushButton::clicked, &niveau_hf1_, &Niveau::SIGNAL_SetStartupFinalValue);
    QObject::connect(ui_->pushButton_setshutdowninitialvalue, &QPushButton::clicked, &niveau_hf1_, &Niveau::SIGNAL_SetShutdownInitialValue);
    QObject::connect(&niveau_hf1_, &Niveau::SIGNAL_Value, this, [&](double value) { ui_->label_niveau->setText(QString::number(value)); });
    QObject::connect(&niveau_hf1_, &Niveau::SIGNAL_On, this, [&]() { ui_->radiobutton_on->setChecked(true); });
    QObject::connect(&niveau_hf1_, &Niveau::SIGNAL_Off, this, [&]() { ui_->radiobutton_on->setChecked(false); });

    QObject::connect(ui_->pushButton_interrupt_2, &QPushButton::clicked, [&]() { niveau_hf2_.Interrupt(); });
    QObject::connect(ui_->pushButton_setlevel_2, &QPushButton::clicked, [&]() { niveau_hf2_.SetLevel(ui_->doubleSpinBox_value_2->value()); });
    QObject::connect(ui_->pushButton_niveau_on_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::On);
    QObject::connect(ui_->pushButton_niveau_off_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::Off);
    QObject::connect(ui_->pushButton_niveau_inc_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::Increment);
    QObject::connect(ui_->pushButton_niveau_dec_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::Decrement);
    QObject::connect(ui_->pushButton_setstartupfinalvalue_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::SIGNAL_SetStartupFinalValue);
    QObject::connect(ui_->pushButton_setshutdowninitialvalue_2, &QPushButton::clicked, &niveau_hf2_, &Niveau::SIGNAL_SetShutdownInitialValue);
    QObject::connect(&niveau_hf2_, &Niveau::SIGNAL_Value, this, [&](double value) { ui_->label_niveau_2->setText(QString::number(value)); });
    QObject::connect(&niveau_hf2_, &Niveau::SIGNAL_On, this, [&]() { ui_->radiobutton_on_2->setChecked(true); });
    QObject::connect(&niveau_hf2_, &Niveau::SIGNAL_Off, this, [&]() { ui_->radiobutton_on_2->setChecked(false); });


}

}
