#include "MembraneGui.h"
#include "ui_MembraneGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

MembraneGui::MembraneGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::MembraneGui),
    membrane_hf1_(HFX::HF1),
    membrane_hf2_(HFX::HF2)
{
    qDebug() << "MembraneGui::MembraneGui";
    ui_->setupUi(this);
    ConnectSignals();
}

MembraneGui::~MembraneGui()
{
}

void MembraneGui::ConnectSignals() {
    qDebug() << "MembraneGui::ConnectSignals";

    QObject::connect(ui_->pushButton_movetoaccord, &QPushButton::clicked, &membrane_hf1_, &Membrane::MoveToAccordPosition);
    QObject::connect(ui_->pushButton_saveaccordpos, &QPushButton::clicked, &membrane_hf1_, &Membrane::SaveAccordPosition);
    QObject::connect(ui_->pushButton_membrane_moveup, &QPushButton::clicked, &membrane_hf1_, &Membrane::StartMovingUp);
    QObject::connect(ui_->pushButton_membrane_movedown, &QPushButton::clicked, &membrane_hf1_, &Membrane::StartMovingDown);
    QObject::connect(ui_->pushButton_membrane_stop, &QPushButton::clicked, &membrane_hf1_, &Membrane::Stop);
    QObject::connect(ui_->pushButton_membrane_manu, &QPushButton::clicked, &membrane_hf1_, &Membrane::SetModeManual);
    QObject::connect(ui_->pushButton_membrane_auto, &QPushButton::clicked, &membrane_hf1_, &Membrane::SetModeAutomatic);
    QObject::connect(&membrane_hf1_, &Membrane::SIGNAL_Automatic, this, [&]() { ui_->radioButton_membrane_mode_auto->setChecked(true); });
    QObject::connect(&membrane_hf1_, &Membrane::SIGNAL_Manual, this, [&]() { ui_->radioButton_membrane_mode_auto->setChecked(false); });
    QObject::connect(&membrane_hf1_, &Membrane::SIGNAL_Position, this, [&](double pos) { ui_->label_membrane_position->setText(QString::number(pos)); });

    QObject::connect(ui_->pushButton_movetoaccord_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::MoveToAccordPosition);
    QObject::connect(ui_->pushButton_saveaccordpos_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::SaveAccordPosition);
    QObject::connect(ui_->pushButton_membrane_moveup_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::StartMovingUp);
    QObject::connect(ui_->pushButton_membrane_movedown_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::StartMovingDown);
    QObject::connect(ui_->pushButton_membrane_stop_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::Stop);
    QObject::connect(ui_->pushButton_membrane_manu_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::SetModeManual);
    QObject::connect(ui_->pushButton_membrane_auto_2, &QPushButton::clicked, &membrane_hf2_, &Membrane::SetModeAutomatic);
    QObject::connect(&membrane_hf2_, &Membrane::SIGNAL_Automatic, this, [&]() { ui_->radioButton_membrane_mode_auto_2->setChecked(true); });
    QObject::connect(&membrane_hf2_, &Membrane::SIGNAL_Manual, this, [&]() { ui_->radioButton_membrane_mode_auto_2->setChecked(false); });
    QObject::connect(&membrane_hf2_, &Membrane::SIGNAL_Position, this, [&](double pos) { ui_->label_membrane_position_2->setText(QString::number(pos)); });

}

}
