#include "ConfigSaveDialog.h"
#include "ui_ConfigSaveDialog.h"

#include <QMessageBox>
#include <QTableWidget>

namespace medicyc::cyclotroncontrolsystem::ui::main {

ConfigSaveDialog::ConfigSaveDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::ConfigSaveDialog)
{
    ui_->setupUi(this);
    setWindowTitle("Sauvegarder configurations");
    ConnectSignals();
}

ConfigSaveDialog::~ConfigSaveDialog()
{
    delete ui_;
}

void ConfigSaveDialog::ConnectSignals() {
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, [=]() { done(2); });
    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, &ConfigSaveDialog::CheckNameValidity);
}

void ConfigSaveDialog::CheckNameValidity() {
    if (ui_->lineEdit_name->text().isEmpty() || ui_->lineEdit_name->text().isNull()) {
        QMessageBox::warning(this, "Cyclotron", QString("Le nom ne peut pas être vide"));
        return;
    }
    name_ = ui_->lineEdit_name->text();
    comments_ = ui_->lineEdit_comments->text();
    accept();
}

}
