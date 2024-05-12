#include "SignalGeneratorGui.h"
#include "ui_SignalGeneratorGui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>

SignalGeneratorGui::SignalGeneratorGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::SignalGeneratorGui) {

    ui_->setupUi(this);
    setWindowTitle("Signal Generator");
    ShowDisconnected();
    //QObject::connect(&adc_, SIGNAL(SIGNAL_IOError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));

    QObject::connect(&signal_generator_, &hw_signalgenerator::SignalGenerator::SIGNAL_Connected, this, &SignalGeneratorGui::ShowConnected);
    QObject::connect(&signal_generator_, &hw_signalgenerator::SignalGenerator::SIGNAL_Disconnected, this, &SignalGeneratorGui::ShowDisconnected);
    QObject::connect(&signal_generator_, &hw_signalgenerator::SignalGenerator::SIGNAL_ReplyReceived, this, &SignalGeneratorGui::ShowPing);
    QObject::connect(&signal_generator_, &hw_signalgenerator::SignalGenerator::SIGNAL_Address, this, &SignalGeneratorGui::ShowAddress);
    QObject::connect(&signal_generator_, &hw_signalgenerator::SignalGenerator::SIGNAL_Frequency, this, [&](double f) {
        ui_->lineEdit_frequency->setText(QString::number(f, 'f', 3));
    });
    QObject::connect(ui_->pushButton_set_address, &QPushButton::clicked, this, &SignalGeneratorGui::SetAddress);
    QObject::connect(ui_->pushButton_read_address, &QPushButton::clicked, &signal_generator_, &hw_signalgenerator::SignalGenerator::ReadAddress);
    QObject::connect(ui_->pushButton_init, &QPushButton::clicked, &signal_generator_, &hw_signalgenerator::SignalGenerator::Init);
    QObject::connect(ui_->pushButton_amplitude_setabsolut, &QPushButton::clicked, this, &SignalGeneratorGui::SetAbsoluteAmplitude);
    QObject::connect(ui_->pushButton_frequency_setabsolut, &QPushButton::clicked, this, &SignalGeneratorGui::SetAbsoluteFrequency);
    QObject::connect(ui_->pushButton_frequency_setstep, &QPushButton::clicked, this, &SignalGeneratorGui::SetStepFrequency);
    QObject::connect(ui_->pushButton_frequency_up, &QPushButton::clicked, &signal_generator_, &hw_signalgenerator::SignalGenerator::IncrementFrequency);
    QObject::connect(ui_->pushButton_frequency_down, &QPushButton::clicked, &signal_generator_, &hw_signalgenerator::SignalGenerator::DecrementFrequency);
}

SignalGeneratorGui::~SignalGeneratorGui()
{
}

void SignalGeneratorGui::ShowAddress(int address) {
    ui_->lineEdit_read_address->setText(QString::number(address));
}

void SignalGeneratorGui::SetAddress() {
    int address = ui_->lineEdit_set_address->text().toInt();
    signal_generator_.SetAddress(address);
}

void SignalGeneratorGui::SetAbsoluteAmplitude() {
    double amplitude = ui_->lineEdit_amplitude->text().toDouble();
    signal_generator_.SetAmplitude(amplitude);
}

void SignalGeneratorGui::SetAbsoluteFrequency() {
    double frequency = ui_->lineEdit_frequency_absolute->text().toDouble();
    signal_generator_.SetFrequency(frequency);
}

void SignalGeneratorGui::SetStepFrequency() {
    double step = ui_->lineEdit_frequency_step->text().toDouble();
    signal_generator_.SetFrequencyStep(step);
}

void SignalGeneratorGui::ShowPing() {
    ui_->frame_ping->setStyleSheet(FrameStyleSheet("frame_ping", "circle-blue-20x20"));
    QTimer::singleShot(500, this, [&]() { ui_->frame_ping->setStyleSheet("QFrame {}"); });
}

void SignalGeneratorGui::ShowConnected() {
    qDebug() << "ShowConnected";
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void SignalGeneratorGui::ShowDisconnected() {
    qDebug() << "ShowDisconnected";
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}

QString SignalGeneratorGui::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}
