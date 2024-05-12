#include "Gui.h"
#include "ui_Gui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>
#include "DBus.h"
#include "Parameters.h"

Gui::Gui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::Gui),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat)))
{
    ui_->setupUi(this);
    setWindowTitle("MessageLogger");
    SetupLayout();
    SetupConnection();
}

Gui::~Gui()
{
    delete iserver_;
}

void Gui::SetupConnection() {
    iserver_ = new medicyc::cyclotron::MessageLoggerInterface("medicyc.cyclotron.messagelogger", "/MessageLogger",
                                                              medicyc::cyclotroncontrolsystem::global::GetDBusConnection());

    QObject::connect(iserver_, &MessageLoggerInterface::SIGNAL_Error, this, &Gui::DisplayError);
    QObject::connect(iserver_, &MessageLoggerInterface::SIGNAL_Warning, this, &Gui::DisplayWarning);
    QObject::connect(iserver_, &MessageLoggerInterface::SIGNAL_Info, this, &Gui::DisplayInfo);

    QObject::connect(ui_->pushButton_clear, &QPushButton::clicked, textfield_, &QPlainTextEdit::clear);
}

void Gui::SetupLayout() {
    textfield_ = new QPlainTextEdit();
    ui_->scrollArea->setWidget(textfield_);
}


QString Gui::GetFormattedMessage(qlonglong datetime, const QString& system, const QString& message) const {
    return QDateTime::fromMSecsSinceEpoch(datetime).toString("hh:mm:ss.zzz") + "     " + system + "\t   " + message;
}

void Gui::DisplayError(qlonglong datetime, const QString &system, const QString &message) {
    DisplayMessage("Error\t" + GetFormattedMessage(datetime, system, message), Qt::red);
}

void Gui::DisplayWarning(qlonglong datetime, const QString &system, const QString &message) {
    DisplayMessage("Warning\t" + GetFormattedMessage(datetime, system, message), Qt::yellow);
}

void Gui::DisplayInfo(qlonglong datetime, const QString &system, const QString &message) {
    DisplayMessage("Info\t" + GetFormattedMessage(datetime, system, message), Qt::white);
}

void Gui::DisplayMessage(QString line, QColor color) {
    QTextCharFormat tf;
    tf = textfield_->currentCharFormat();
    tf.setTextOutline(QPen(color));
    textfield_->setCurrentCharFormat(tf);
    textfield_->appendPlainText(line);

}
