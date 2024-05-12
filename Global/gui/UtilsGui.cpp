#include "UtilsGui.h"
#include "ui_UtilsGui.h"

#include "CountedState.h"

#include <QStandardPaths>
#include <QSignalTransition>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::global {

UtilsGui::UtilsGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::UtilsGui)
{
    ui_->setupUi(this);

    SetupStateMachine();
}

UtilsGui::~UtilsGui() {

}

void UtilsGui::SetupStateMachine() {

    // States
    QState *sSuperState = new QState();
    QState *sIdle = new QState(sSuperState);
    QState *sError = new QState(sSuperState);
    MonitoredState *sDoWork = new MonitoredState(sSuperState, 3000, "Timing out..", 3, sError);

    QObject::connect(sIdle, &QState::entered, this, [=]() { ui_->label_current_state->setText("Idle Entered"); });
//    QObject::connect(sDoWork, &QState::entered, this, [=]() { ui_->label_current_state->setText("DoWork Entered"); });
    QObject::connect(sError, &QState::entered, this, [=]() { ui_->label_current_state->setText("Error Entered"); });

    QObject::connect(sDoWork, &MonitoredState::SIGNAL_Timeout, this, [&](QString message) { qDebug() << "Timeout " << message; });
    QObject::connect(sDoWork, &MonitoredState::SIGNAL_Bailout, this, [&]() { qDebug() << "Bailout "; });
    QObject::connect(sDoWork, &MonitoredState::SIGNAL_DoWork, this, [&]() { ui_->label_current_state->setText(QDateTime::currentDateTime().toString()); });

    sSuperState->setInitialState(sIdle);
    sIdle->addTransition(ui_->pushButton_transition, &QPushButton::clicked, sDoWork);
    sDoWork->addTransition(ui_->pushButton_transition, &QPushButton::clicked, sIdle);
    sSuperState->addTransition(ui_->pushButton_reset, &QPushButton::clicked, sSuperState);

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}


}
