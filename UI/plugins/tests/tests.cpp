#include "tests.h"

#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QStandardPaths>
#include "ClickableGeometricShape.h"

tests::tests(QWidget *parent) :
    QMainWindow(parent),
    ui_(new Ui::tests)
{
    ui_->setupUi(this);
    setWindowTitle(QString("tests"));
    this->show(); // needed?

    ui_->widget->SetupAsCenterFilling(Qt::blue);
    ui_->widget_2->SetupAsColorChanging(Qt::green, Qt::red);

    ui_->widget->Enable();
    ui_->widget_2->Enable();
    std::vector<double> duration = {100, 300, 300, 300};
    std::vector<double> levels = {800, 1000, 1200, 1050};


    QObject::connect(ui_->widget_3, &ClickableGeometricShape::SIGNAL_Clicked, this, [&]() { qDebug() << "Clicked"; });
    ui_->widget_pole->Configure(3, 10);
    QObject::connect(ui_->verticalSlider, &QSlider::valueChanged, this, [&](int value) { ui_->widget_pole->SetLevel(value); });

    QObject::connect(ui_->pushButton_setvalue, &QPushButton::clicked, this, [&]() { ui_->widget_pole->SetLevel(ui_->lineEdit_value->text().toDouble()); });

    QObject::connect(ui_->pushButton_turnoncenter, &QPushButton::clicked, ui_->widget, &GenericDisplayButton::On);
    QObject::connect(ui_->pushButton_turnoffcenter, &QPushButton::clicked, ui_->widget, &GenericDisplayButton::Off);
    QObject::connect(ui_->pushButton_turnoncenter, &QPushButton::clicked, ui_->widget_2, &GenericDisplayButton::On);
    QObject::connect(ui_->pushButton_turnoffcenter, &QPushButton::clicked, ui_->widget_2, &GenericDisplayButton::Off);

    QObject::connect(ui_->pushButton_disable , &QPushButton::clicked, ui_->widget, &GenericDisplayButton::Disable);
    QObject::connect(ui_->pushButton_enable, &QPushButton::clicked, ui_->widget, &GenericDisplayButton::Enable);
    QObject::connect(ui_->pushButton_disable, &QPushButton::clicked, ui_->widget_2, &GenericDisplayButton::Disable);
    QObject::connect(ui_->pushButton_enable, &QPushButton::clicked, ui_->widget_2, &GenericDisplayButton::Enable);

    QObject::connect(ui_->pushButton_add, &QPushButton::clicked, this, [&]() {
        ui_->widget_histogram->AddData(ui_->doubleSpinBox_key->value(), ui_->doubleSpinBox_value->value()); });
}

tests::~tests() {
}
