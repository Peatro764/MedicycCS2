#include "ProcessMonitorUI.h"

#include <QGridLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QMessageBox>

#include "GenericDisplayButton.h"
#include "Style.h"
#include "Utils.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

ProcessMonitorUI::ProcessMonitorUI(QWidget *parent) :
    QWidget(parent),
    launcher_interface_(medicyc::cyclotron::LauncherInterface("medicyc.cyclotron.launcher", "/Launcher", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())),
    messagelogger_interface_(medicyc::cyclotron::MessageLoggerInterface("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    setWindowTitle("Processus");
    setWindowFlags(Qt::WindowStaysOnTopHint);

    // Central grid (processes)
    grid_layout_ = new QGridLayout;

    // Bottom rows (pushbuttons)
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    auto pushButton_start_all =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_stop_all =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_start_all, &QPushButton::clicked, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Démarrer tous les processus?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) { launcher_interface_.StartRealRequiredProcesses(); }
    });
    QObject::connect(pushButton_stop_all, &QPushButton::clicked, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Arrêter tous les processus?", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) { launcher_interface_.StopProcesses(); }
    });
    bottom_layout->addWidget(pushButton_start_all);
    bottom_layout->addWidget(pushButton_stop_all);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(grid_layout_);
    v_layout->addLayout(bottom_layout);

    QObject::connect(this, &ProcessMonitorUI::SIGNAL_ToggleProcessState, &launcher_interface_, &medicyc::cyclotron::LauncherInterface::ToggleProcessState);
    QObject::connect(&launcher_interface_, &medicyc::cyclotron::LauncherInterface::SIGNAL_ProcessCreated, this, &ProcessMonitorUI::ProcessCreated);
    QObject::connect(&launcher_interface_, &medicyc::cyclotron::LauncherInterface::SIGNAL_ProcessStarted, this, &ProcessMonitorUI::ProcessStarted);
    QObject::connect(&launcher_interface_, &medicyc::cyclotron::LauncherInterface::SIGNAL_ProcessStopped, this, &ProcessMonitorUI::ProcessStopped);
    QObject::connect(&launcher_interface_, &medicyc::cyclotron::LauncherInterface::SIGNAL_AllRequiredRunning, this, [&]() { emit SIGNAL_RequiredProcessesRunning(true); });
    QObject::connect(&launcher_interface_, &medicyc::cyclotron::LauncherInterface::SIGNAL_AllRequiredNotRunning, this, [&]() { emit SIGNAL_RequiredProcessesRunning(false); });
    QTimer::singleShot(100, this, [&]() { launcher_interface_.GetRealProcesses(); });
}

ProcessMonitorUI::~ProcessMonitorUI() {
    delete grid_layout_;
    for (auto &e : uielements_.toStdMap())  {
        delete e.second;
    }
}

void ProcessMonitorUI::ProcessCreated(const QString& name) {
    qDebug() << "ProcessMonitorUI::ProcessCreated " << name;
    ProcessUIElements *e = GetProcessUIElements(name);
    uielements_[name] = e;
    QObject::connect(e->toggle , &QPushButton::clicked, this, [this, name]() { emit SIGNAL_ToggleProcessState(name); });
    QHBoxLayout *process_layout = new QHBoxLayout;
    process_layout->addWidget(e->allumee, Qt::AlignHCenter);
    process_layout->addWidget(e->toggle, Qt::AlignHCenter);
    process_layout->addWidget(e->name, Qt::AlignLeft);
    grid_layout_->addLayout(process_layout, grid_layout_active_row_, grid_layout_active_col_, Qt::AlignCenter);
    if (grid_layout_active_row_ >= GRIDLAYOUTMAXNROWS) {
        grid_layout_active_row_ = 0;
        grid_layout_active_col_++;
    } else {
        grid_layout_active_row_++;
    }
}

ProcessUIElements* ProcessMonitorUI::GetProcessUIElements(QString name) {
    ProcessUIElements *e = new ProcessUIElements;
    e->allumee = new GenericDisplayButton(this, 25);
    e->allumee->SetupAsCenterFilling(QRgb(0x209fdf));
    e->toggle = new QPushButton("T");
    e->toggle->setFixedWidth(25);
    e->name = shared::utils::GetLabel(name, shared::FONT::SMALL_HEADER);
    //e->name->setFixedWidth(200);
    return e;
}

void ProcessMonitorUI::ProcessStarted(QString name) {
    if (uielements_.contains(name)) {
        uielements_.value(name)->allumee->On();
    } else {
        qDebug() << "ProcessMonitorUI::ProcessStarted Process does not exist " << name;
    }
}

void ProcessMonitorUI::ProcessStopped(QString name) {
    messagelogger_interface_.Error(QDateTime::currentMSecsSinceEpoch(), "MainGui", QString("Un processus s'est arrêté : ") + name);
    if (uielements_.contains(name)) {
        uielements_.value(name)->allumee->Off();
    } else {
        qDebug() << "ProcessMonitorUI::ProcessStopped Process does not exist " << name;
    }
}

}
