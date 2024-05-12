#include "ConfigSelectionDialog.h"
#include "ui_ConfigSelectionDialog.h"

#include <QMessageBox>
#include <QTableWidget>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::main {

// QStringList = {name, comments}
ConfigSelectionDialog::ConfigSelectionDialog(QWidget *parent, QMap<QDateTime, QStringList> available_configs) :
    QDialog(parent),
    ui_(new Ui::ConfigSelectionDialog),
    available_configs_(available_configs)
{
    ui_->setupUi(this);
    setWindowTitle("Configurations disponibles");
    SetupTable();
    FillTable();
    ConnectSignals();
}

ConfigSelectionDialog::~ConfigSelectionDialog()
{
    delete ui_;
}

void ConfigSelectionDialog::ConnectSignals() {
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, [=]() { done(2); });
    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, &ConfigSelectionDialog::GetSelectedConfig);
}

void ConfigSelectionDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Horodatage" << "Nom" << "Commentaires";
    ui_->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::TIMESTAMP), 200);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::NAME), 120);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::COMMENTS), 620);
    ui_->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ConfigSelectionDialog::FillTable() {
    ui_->tableWidget->clearContents();
    ui_->tableWidget->setRowCount(static_cast<int>(available_configs_.size()));
    auto keys = available_configs_.keys();
    int row = 0;
    for (auto &k : keys) {
        auto name_comments = available_configs_.value(k);
        if (name_comments.size() == 2) {
            FillRow(row++, k, name_comments.at(0), name_comments.at(1));
        } else {
            qDebug() << "Wrong number of items in stringlist: " << name_comments.size() << ", should be 2 (name, comments)";
        }
    }
    ui_->tableWidget->setCurrentCell(0, 0);
}

void ConfigSelectionDialog::FillRow(int row, QDateTime timestamp, QString name, QString comments) {
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::TIMESTAMP), new QTableWidgetItem(timestamp.toString()));
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::NAME), new QTableWidgetItem(name));
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::COMMENTS), new QTableWidgetItem(comments));
}



void ConfigSelectionDialog::GetSelectedConfig() {
    QTableWidgetItem *item = ui_->tableWidget->item(ui_->tableWidget->currentRow(), static_cast<int>(COLUMNS::NAME));
    if (item) {
        selected_config_ = item->data(Qt::DisplayRole).toString();
        accept();
    } else {
        QMessageBox::warning(this, "CycloControl", QString("Configuration non trouvé"));
    }
}

}
