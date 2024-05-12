#include "OmronSelectionDialog.h"
#include "ui_OmronSelectionDialog.h"

#include <QMessageBox>
#include <QTableWidget>

namespace medicyc::cyclotroncontrolsystem::ui::main {

OmronSelectionDialog::OmronSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::OmronSelectionDialog)
{
    ui_->setupUi(this);
    setWindowTitle("Sélectionnez une interface Omron");
    QObject::connect(ui_->pushButton_quit, &QPushButton::clicked, this, &QDialog::reject);
    QObject::connect(ui_->pushButton_open, &QPushButton::clicked, this, &QDialog::accept);
}

OmronSelectionDialog::~OmronSelectionDialog()
{
    delete ui_;
}

QStringList OmronSelectionDialog::Chosen() {
    QStringList args;
    if (ui_->pushButton_Cooling->isChecked()) args.push_back("Cooling");
    if (ui_->pushButton_VDF->isChecked()) args.push_back("VDF");
    if (ui_->pushButton_HF->isChecked()) args.push_back("HF");
    if (ui_->pushButton_Source->isChecked()) args.push_back("Source");
    return args;
}

}
