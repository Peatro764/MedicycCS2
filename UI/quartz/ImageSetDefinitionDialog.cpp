#include "ImageSetDefinitionDialog.h"
#include "ui_ImageSetDefinitionDialog.h"

#include <QMessageBox>
#include <QTableWidget>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

ImageSetDefinitionDialog::ImageSetDefinitionDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::ImageSetDefinitionDialog)
{
    ui_->setupUi(this);
    setWindowTitle("");
    ConnectSignals();
}

ImageSetDefinitionDialog::~ImageSetDefinitionDialog()
{
    delete ui_;
}

void ImageSetDefinitionDialog::ConnectSignals() {
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, [=]() { done(0); });
    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, [=]() { done(1); });
}

QString ImageSetDefinitionDialog::identifier() const {
    return ui_->lineEdit_identifier->text();
}

QString ImageSetDefinitionDialog::comments() const {
    return ui_->lineEdit_comments->text();
}

}
