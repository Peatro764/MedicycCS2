#include "LauncherGui.h"
#include "ui_LauncherGui.h"

#include <QDateTime>
#include <QTimer>
#include <QtDBus/QDBusConnection>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::launcher {

LauncherGui::LauncherGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::LauncherGui),
    launcher_interface_(medicyc::cyclotron::LauncherInterface("medicyc.cyclotron.launcher", "/Launcher", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    ui_->setupUi(this);
    SetupLayout();
    setWindowTitle("Launcher");
    ConnectSignals();
    QTimer::singleShot(500, this, [&]() { launcher_interface_.GetProcesses(); });
}

LauncherGui::~LauncherGui() {
//    launcher_interface_.StopProcesses();
    if (gridLayout_real_buttons_) delete gridLayout_real_buttons_;
    if (gridLayout_sim_buttons_) delete gridLayout_sim_buttons_;
}

void LauncherGui::ConnectSignals() {
    QObject::connect(ui_->pushButton_start_all, &QPushButton::clicked, this, [&]() {
        if (ui_->tabWidget->currentIndex() == 0) {
            launcher_interface_.StartRealRequiredProcesses();
        } else {
            launcher_interface_.StartSimProcesses();
        }
    });
    QObject::connect(ui_->pushButton_stop, &QPushButton::clicked, &launcher_interface_, &cyclotron::LauncherInterface::StopProcesses);
    QObject::connect(&launcher_interface_, &cyclotron::LauncherInterface::SIGNAL_ProcessCreated, this, &LauncherGui::CreateProcessPushButton);
//    QObject::connect(&launcher_, &Launcher::SIGNAL_ProcessDeleted, this, &LauncherGui::DeleteProcessPushButton);
    QObject::connect(&launcher_interface_, &cyclotron::LauncherInterface::SIGNAL_ProcessStarted, this, &LauncherGui::ProcessStarted);
    QObject::connect(&launcher_interface_, &cyclotron::LauncherInterface::SIGNAL_ProcessStopped, this, &LauncherGui::ProcessStopped);

    QObject::connect(&launcher_interface_, &cyclotron::LauncherInterface::SIGNAL_AllRequiredRunning, this, [&]() {
        ui_->pushButton_all_required_running->setStyleSheet(ButtonStyleSheet(true));
    });
    QObject::connect(&launcher_interface_, &cyclotron::LauncherInterface::SIGNAL_AllRequiredNotRunning, this, [&]() {
        ui_->pushButton_all_required_running->setStyleSheet(ButtonStyleSheet(false));
    });
}

void LauncherGui::CreateProcessPushButton(QString name) {
    if (process_buttons_.contains(name)) {
        qDebug() << "LauncherGui::CreateProcessPushButton Process already exist " << name;
        return;
    }
    QPushButton *button = GetNewButton(name);
    process_buttons_[name] = button;
    if (name.startsWith("Sim_")) {
        gridLayout_sim_buttons_->addWidget(button, n_sim_processes_ / NCOL, n_sim_processes_ % NCOL, Qt::AlignHCenter);
        ++n_sim_processes_;
    } else {
        gridLayout_real_buttons_->addWidget(button, n_real_processes_ / NCOL, n_real_processes_ % NCOL, Qt::AlignHCenter);
        ++n_real_processes_;
    }
    QObject::connect(button, &QPushButton::clicked, this, [this, name](){ launcher_interface_.ToggleProcessState(name); });
}

void LauncherGui::ProcessStarted(QString name) {
    if (!process_buttons_.contains(name)) {
        qDebug() << "LauncherGui::ProcessStarted Process does not exist " << name;
        return;
    }
    auto button = process_buttons_.value(name);
    button->setStyleSheet(ButtonStyleSheet(true));
}

void LauncherGui::ProcessStopped(QString name) {
    if (!process_buttons_.contains(name)) {
        qDebug() << "LauncherGui::ProcessStopped Process does not exist " << name;
        return;
    }
    auto button = process_buttons_.value(name);
    button->setStyleSheet(ButtonStyleSheet(false));
}

void LauncherGui::SetupLayout() {
    gridLayout_real_buttons_ = new QGridLayout(this);
    ui_->tab_real->setLayout(gridLayout_real_buttons_);

    gridLayout_sim_buttons_ = new QGridLayout(this);
    ui_->tab_simulated->setLayout(gridLayout_sim_buttons_);

    ui_->pushButton_all_required_running->setStyleSheet(ButtonStyleSheet(false));
}

QPushButton* LauncherGui::GetNewButton(QString text) {
    QPushButton *button = new QPushButton(text);
    button->setFixedWidth(300);
    button->setMinimumWidth(300);
    button->setFixedHeight(50);
    button->setMinimumHeight(50);
    QFont font("Ubuntu", 13);
    font.setBold(true);
    button->setFont(font);
    button->setStyleSheet(ButtonStyleSheet(false));
    return button;
}

QString LauncherGui::ButtonStyleSheet(bool on) const {
    QString stylesheet;
    stylesheet.push_back("QPushButton {");
    stylesheet.push_back("color: white;");
    if (on) {
        stylesheet.push_back("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 rgb(138, 226, 52), stop: 0.5 rgb(78, 154, 6));");
    } else {
        stylesheet.push_back("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 rgb(239, 41, 41), stop: 0.5 rgb(164, 0, 0));");
    }
    stylesheet.push_back("border-width: 0.3ex;");
    stylesheet.push_back("border-color: #76797c;");
    stylesheet.push_back("border-style: solid;");
    stylesheet.push_back("padding: 0.5ex;");
    stylesheet.push_back("border-radius: 0.2ex;");
    stylesheet.push_back("outline: none;");
    stylesheet.push_back("}");
    stylesheet.push_back("QPushButton:hover {");
    stylesheet.push_back("border: 0.4ex solid white;");
    stylesheet.push_back("}");

    return stylesheet;
}


} // namespace

