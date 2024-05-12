#include "ImageSetSelectionDialog.h"
#include "ui_ImageSetSelectionDialog.h"

#include <QMessageBox>
#include <QTableWidget>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

ImageSetSelectionDialog::ImageSetSelectionDialog(QWidget *parent, QMap<QDateTime, QString> available_imagesets) :
    QDialog(parent),
    ui_(new Ui::ImageSetSelectionDialog),
    available_imagesets_(available_imagesets)
{
    ui_->setupUi(this);
    setWindowTitle("Série d'images disponibles");
    SetupTable();
    FillTable();
    ConnectSignals();
}

ImageSetSelectionDialog::~ImageSetSelectionDialog()
{
    delete ui_;
}

void ImageSetSelectionDialog::ConnectSignals() {
    QObject::connect(ui_->pushButton_cancel, &QPushButton::clicked, this, [=]() { done(2); });
    QObject::connect(ui_->pushButton_ok, &QPushButton::clicked, this, &ImageSetSelectionDialog::GetSelectedImageSet);
}

void ImageSetSelectionDialog::SetupTable() {
    QStringList headerLabels;
    headerLabels << "Horodatage" << "Nom" << "Commentaires";
    ui_->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::TIMESTAMP), 200);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::NAME), 120);
    ui_->tableWidget->setColumnWidth(static_cast<int>(COLUMNS::COMMENTS), 280);
    ui_->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void ImageSetSelectionDialog::FillTable() {
    ui_->tableWidget->clearContents();
    ui_->tableWidget->setRowCount(static_cast<int>(available_imagesets_.size()));
    auto keys = available_imagesets_.keys();
    int row = 0;
    for (auto k : keys) {
        FillRow(row++, k, available_imagesets_.value(k), QString("..."));
    }
    ui_->tableWidget->setCurrentCell(0, 0);
}

void ImageSetSelectionDialog::FillRow(int row, QDateTime timestamp, QString name, QString comments) {
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::TIMESTAMP), new QTableWidgetItem(timestamp.toString()));
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::NAME), new QTableWidgetItem(name));
    ui_->tableWidget->setItem(row, static_cast<int>(COLUMNS::COMMENTS), new QTableWidgetItem(comments));
}



void ImageSetSelectionDialog::GetSelectedImageSet() {
    QTableWidgetItem *item = ui_->tableWidget->item(ui_->tableWidget->currentRow(), static_cast<int>(COLUMNS::NAME));
    if (item) {
        selected_imageset_ = item->data(Qt::DisplayRole).toString();
        accept();
    } else {
        QMessageBox::warning(this, "CycloControl", QString("Série d'images non trouvé"));
    }
}

}
