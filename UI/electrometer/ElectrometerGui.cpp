#include "ElectrometerGui.h"
#include "ui_ElectrometerGui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::ui::electrometer {

ElectrometerGui::ElectrometerGui(QWidget *parent, QString name) :
    QWidget(parent),
    ui_(new Ui::ElectrometerGui),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
    electrometer_interface_("medicyc.cyclotron.hardware.electrometer." + name, "/" + name, medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this)
{
    ui_->setupUi(this);
    setWindowTitle("Electrometer");
    ShowDisconnected();
    ShowLeakageCurrentSelection(false);
    //QObject::connect(&adc_, SIGNAL(SIGNAL_IOError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));

    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Connected, this, &ElectrometerGui::ShowConnected);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Disconnected, this, &ElectrometerGui::ShowDisconnected);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_ReplyReceived, this, &ElectrometerGui::ShowPing);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Measurement, this, &ElectrometerGui::ShowMeasurement);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Measurement, this, &ElectrometerGui::PlotMeasurement);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_ErrorMessage, this, [&](QString message) { UserErrorPopup(message); });

    QObject::connect(ui_->pushButton_quit, &QPushButton::clicked, this, [&]() { exit(1); });
    QObject::connect(ui_->pushButton_20pA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(20e-12); });
    QObject::connect(ui_->pushButton_200pA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(200e-12); });
    QObject::connect(ui_->pushButton_2nA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(2e-9); });
    QObject::connect(ui_->pushButton_20nA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(20e-9); });
    QObject::connect(ui_->pushButton_200nA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(200e-9); });
    QObject::connect(ui_->pushButton_2uA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(2e-6); });
    QObject::connect(ui_->pushButton_20uA, &QPushButton::clicked, this, [&]() { electrometer_interface_.SetRange(20e-6); });

    QObject::connect(ui_->pushButton_remove_zero, &QPushButton::clicked, &electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::RemoveZero);
    QObject::connect(ui_->pushButton_setzero, &QPushButton::clicked, &electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SetZero);
    QObject::connect(ui_->pushButton_raz, &QPushButton::clicked, &electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::RAZ);

    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_ZeroRemoved, this, [&]() { ShowZero(0.0); });
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Zero, this, &ElectrometerGui::ShowZero);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Zero, this, [&]() { ShowLeakageCurrentSelection(true); });
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_ZeroRemoved, this, [&]() { ShowLeakageCurrentSelection(false); });
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_ZeroForcedRemoved, this, [&]() { UserInfoPopup(QString("Le courant de fuite a été mis à zéro lors du changement de calibre")); });
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_CurrentRange, this, &ElectrometerGui::ShowCurrentRange);

    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Mean, this, &ElectrometerGui::ShowMean);
    QObject::connect(&electrometer_interface_, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_StdDev, this, &ElectrometerGui::ShowStdDev);

    sliding_graph_ = new SlidingGraph(ui_->chartview);
    QObject::connect(ui_->pushButton_raz, &QPushButton::clicked, sliding_graph_, &SlidingGraph::Clear);
    QObject::connect(ui_->pushButton_raz, &QPushButton::clicked, this, [&]() { sliding_graph_->SetYRange(1e-13, 1e-12); });

    QTimer::singleShot(1000, this, [&]() { electrometer_interface_.ReadRange(); });
}

ElectrometerGui::~ElectrometerGui()
{
    if (sliding_graph_)  delete sliding_graph_;
}

void ElectrometerGui::ShowMeasurement(double timestamp, double value, bool ol) {
    (void)timestamp;
    QString value_string;
    QString value_beamcurrent;
    if (std::abs(value) < 1e-9) {
        const double scaled_value = value / 1e-12;
        value_string = QString::number(scaled_value, 'f', GetNumberOfDigits(scaled_value));
        value_beamcurrent = QString::number(scaled_value/2, 'f', GetNumberOfDigits(scaled_value/2));
        unit_ = QString("pA");
    } else if (std::abs(value) < 1e-6) {
        const double scaled_value = value / 1e-9;
        value_string = QString::number(scaled_value, 'f', GetNumberOfDigits(scaled_value));
        value_beamcurrent = QString::number(scaled_value/2, 'f', GetNumberOfDigits(scaled_value/2));
        unit_ = QString("nA");
    } else if (std::abs(value) < 1e-3) {
        const double scaled_value = value / 1e-6;
        value_string = QString::number(scaled_value, 'f', GetNumberOfDigits(scaled_value));
        value_beamcurrent = QString::number(scaled_value/2, 'f', GetNumberOfDigits(scaled_value/2));
        unit_ = QString("uA");
    } else {
        value_string = QString("SAT");
        value_beamcurrent = QString("SAT");
        unit_ = QString("-");
    }
    ui_->label_current_value->setText(value_string);
    ui_->label_current_unit->setText(unit_);
    ui_->label_current_faisceau->setText(value_beamcurrent);
    if (ol) {
        ui_->label_current_value->setStyleSheet("color: rgb(237, 51, 59)");
        ui_->label_current_unit->setStyleSheet("color: rgb(237, 51, 59)");
    } else {
        ui_->label_current_value->setStyleSheet("color: rgb(255,255,255)");
        ui_->label_current_unit->setStyleSheet("color: rgb(255,255,255)");
    }
}

void ElectrometerGui::PlotMeasurement(double timestamp, double value, bool ol) {
    (void)ol;
    if (!ol) {
        sliding_graph_->AddMeasurement(timestamp, value);
    }
}

int ElectrometerGui::GetNumberOfDigits(double value) const {
    if (abs(value) < 10.0) {
        return 2;
    } else if (abs(value) < 100) {
        return 2;
    } else {
        return 1;
    }
}

void ElectrometerGui::ShowZero(double value) {
    if (unit_ == "pA") {
        value = value / 1e-12;
    } else if (unit_ == "nA") {
        value = value / 1e-9;
    } else if (unit_ == "uA") {
        value = value / 1e-6;
    } else {
        value = 0;
    }
    ui_->label_current_zero->setText(QString::number(value, 'f', GetNumberOfDigits(value)));
}

void ElectrometerGui::ShowMean(double value) {
    if (unit_ == "pA") {
        value = value / 1e-12;
    } else if (unit_ == "nA") {
        value = value / 1e-9;
    } else if (unit_ == "uA") {
        value = value / 1e-6;
    } else {
        value = 0;
    }
    QString number_as_string = QString::number(value, 'f', GetNumberOfDigits(value));
    ui_->label_current_average->setText(number_as_string);
}

void ElectrometerGui::ShowStdDev(double value) {
    if (unit_ == "pA") {
        value = value / 1e-12;
    } else if (unit_ == "nA") {
        value = value / 1e-9;
    } else if (unit_ == "uA") {
        value = value / 1e-6;
    } else {
        value = 0;
    }
    ui_->label_current_sd->setText(QString::number(value, 'f', GetNumberOfDigits(value)));
}

void ElectrometerGui::ShowLeakageCurrentSelection(bool activated) {
    QString blue("background-color: #3daee9");
    QString black("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 #3b4045, stop: 0.5 #31363b)");
    if (activated) {
        ui_->pushButton_setzero->setStyleSheet(blue);
        ui_->pushButton_remove_zero->setStyleSheet(black);
    } else {
        ui_->pushButton_setzero->setStyleSheet(black);
        ui_->pushButton_remove_zero->setStyleSheet(blue);
    }
}

void ElectrometerGui::ShowCurrentRange(double value) {
    QString blue("background-color: #3daee9");
    QString black("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 #3b4045, stop: 0.5 #31363b)");
    ui_->pushButton_20pA->setStyleSheet(black);
    ui_->pushButton_200pA->setStyleSheet(black);
    ui_->pushButton_2nA->setStyleSheet(black);
    ui_->pushButton_20nA->setStyleSheet(black);
    ui_->pushButton_200nA->setStyleSheet(black);
    ui_->pushButton_2uA->setStyleSheet(black);
    ui_->pushButton_20uA->setStyleSheet(black);
    ui_->pushButton_greater_20uA->setStyleSheet(black);
    if (abs(value - 20e-12)/20e-12 < 0.1) ui_->pushButton_20pA->setStyleSheet(blue);
    if (abs(value - 200e-12)/200e-12 < 0.1) ui_->pushButton_200pA->setStyleSheet(blue);
    if (abs(value - 2e-9)/2e-9 < 0.1) ui_->pushButton_2nA->setStyleSheet(blue);
    if (abs(value - 20e-9)/20e-9 < 0.1) ui_->pushButton_20nA->setStyleSheet(blue);
    if (abs(value - 200e-9)/200e-9 < 0.1) ui_->pushButton_200nA->setStyleSheet(blue);
    if (abs(value - 2e-6)/2e-6 < 0.1) ui_->pushButton_2uA->setStyleSheet(blue);
    if (abs(value - 20e-6)/20e-6 < 0.1) ui_->pushButton_20uA->setStyleSheet(blue);
    if (value > 30e-6) ui_->pushButton_greater_20uA->setStyleSheet(blue);
}

void ElectrometerGui::ShowPing() {
    ui_->frame_ping->setStyleSheet(FrameStyleSheet("frame_ping", "circle-blue-20x20"));
    QTimer::singleShot(500, this, [&]() { ui_->frame_ping->setStyleSheet("QFrame {}"); });
}

void ElectrometerGui::ShowConnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void ElectrometerGui::ShowDisconnected() {
    ui_->label_current_average->setText("-");
    ui_->label_current_sd->setText("-");
    ui_->label_current_faisceau->setText("-");
    ui_->label_current_value->setText("-");
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}

QString ElectrometerGui::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

bool ElectrometerGui::UserQuestionPopup(QString question) {
    QMessageBox msg;
    msg.setText(question + "?");
//    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setStyleSheet("QLabel{min-width:500px; min-height:120px; font-size: 32px;} QPushButton{ width:250px; height:100px; font-size: 30px; }");

    bool reply(false);
    int ret = msg.exec();
    switch (ret) {
    case QMessageBox::Ok:
        reply = true;
        break;
    case QMessageBox::Cancel:
        reply = false;
        break;
    default:
        qWarning() << "ElectrometerGui::UserQuestionPopup Run into default clause in switch.";
        reply = false;
        break;
    }
    return reply;
}

void ElectrometerGui::UserInfoPopup(QString message) {
    QMessageBox msg;
    msg.setText(message);
//    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet("QLabel{min-width:400px; min-height:120px; font-size: 32px;} QPushButton{ width:400px; height:100px; font-size: 30px; }");
    msg.exec();
}

void ElectrometerGui::UserErrorPopup(QString message) {
    QMessageBox msg;
    msg.setText(message);
//    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet("QLabel{min-width:400px; min-height:120px; font-size: 32px;} QPushButton{ width:400px; height:100px; font-size: 30px; }");
    msg.exec();
}


} // ns
