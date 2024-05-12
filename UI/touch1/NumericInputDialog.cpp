#include "NumericInputDialog.h"
#include "ui_NumericInputDialog.h"

#include <QMessageBox>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

NumericInputDialog::NumericInputDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::NumericInputDialog)
{
    ui_->setupUi(this);

    QObject::connect(ui_->pushButton_0, &QPushButton::clicked, this, [&]() { AddNumber("0"); });
    QObject::connect(ui_->pushButton_1, &QPushButton::clicked, this, [&]() { AddNumber("1"); });
    QObject::connect(ui_->pushButton_2, &QPushButton::clicked, this, [&]() { AddNumber("2"); });
    QObject::connect(ui_->pushButton_3, &QPushButton::clicked, this, [&]() { AddNumber("3"); });
    QObject::connect(ui_->pushButton_4, &QPushButton::clicked, this, [&]() { AddNumber("4"); });
    QObject::connect(ui_->pushButton_5, &QPushButton::clicked, this, [&]() { AddNumber("5"); });
    QObject::connect(ui_->pushButton_6, &QPushButton::clicked, this, [&]() { AddNumber("6"); });
    QObject::connect(ui_->pushButton_7, &QPushButton::clicked, this, [&]() { AddNumber("7"); });
    QObject::connect(ui_->pushButton_8, &QPushButton::clicked, this, [&]() { AddNumber("8"); });
    QObject::connect(ui_->pushButton_9, &QPushButton::clicked, this, [&]() { AddNumber("9"); });
    QObject::connect(ui_->pushButton_comma, &QPushButton::clicked, this, [&]() { AddNumber("."); });
    QObject::connect(ui_->pushButton_clear, &QPushButton::clicked, ui_->label_input_value, &QLabel::clear);
    QObject::connect(ui_->pushButton_validate, &QPushButton::clicked, this, &NumericInputDialog::Validate);
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, &QDialog::reject);
    setWindowTitle("Entrez la consigné");
}

NumericInputDialog::~NumericInputDialog() {
    qDebug() << "NumericInputDialog::~NumericInputDialog";
}

void NumericInputDialog::AddNumber(QString input) {
    QString new_value = ui_->label_input_value->text() + input;
    bool ok(false);
    new_value.toDouble(&ok);
    if (ok) {
        ui_->label_input_value->setText(new_value);
    } else {
        QMessageBox::critical(this, "", "Valeur non valide");
    }
}

void NumericInputDialog::Validate() {
    QString value_string = ui_->label_input_value->text();
    bool ok(false);
    double value = value_string.toDouble(&ok);
    if (ok && !value_string.isEmpty() && !value_string.isNull()) {
        emit SIGNAL_Value(value);
        accept();
    } else {
        QMessageBox::critical(this, "", "Valeur non valide");
    }
}


}
