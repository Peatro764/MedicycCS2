#include "SondePrincipale.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QSound>
#include <QInputDialog>

#include "DBus.h"

SondePrincipale::SondePrincipale(QWidget *parent) :
    QWidget(parent),
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "ADC DBUS " << adc_.isValid();
    QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, &SondePrincipale::InterpretADCChannelRead);

    SetupPlot();

    QLabel *l_value = new QLabel("-");
    l_value->setAlignment(Qt::AlignCenter);
    QLabel *l_pos = new QLabel("-");
    l_pos->setAlignment(Qt::AlignCenter);
    l_value->setMaximumWidth(250);
    l_pos->setMaximumWidth(250);
    QFont font("Ubuntu", 50);
    l_value->setFont(font);
    l_pos->setFont(font);
    QObject::connect(this, &SondePrincipale::SIGNAL_UpdateSeries, this, [&, l_value, l_pos]() {
        l_value->setText(QString::number(current_, 'f', 2));
        l_pos->setText(QString::number(position_, 'f', 2));
    });
    QVBoxLayout *v_layout_values = new QVBoxLayout;
    v_layout_values->addWidget(l_value);
    v_layout_values->addWidget(l_pos);
    QHBoxLayout *h_layout_plot = new QHBoxLayout;
    h_layout_plot->addWidget(&plot_);
    h_layout_plot->addLayout(v_layout_values);

    QPushButton *pushbutton_clear = new QPushButton("Clear");
    pushbutton_clear->setMinimumHeight(40);
    QObject::connect(pushbutton_clear, &QPushButton::clicked, this, &SondePrincipale::ClearGraph);

    QPushButton *pushbutton_start = new QPushButton("Start");
    QPushButton *pushbutton_stop = new QPushButton("Stop");
    QRadioButton *radiobutton_datataking = new QRadioButton("");
    radiobutton_datataking->setStyleSheet("QRadioButton::indicator{ width: 45px; height: 45px; }");
    radiobutton_datataking->setMaximumWidth(50);
    pushbutton_start->setMinimumHeight(40);
    pushbutton_stop->setMinimumHeight(40);
    QHBoxLayout *h_layout_start = new QHBoxLayout;
    h_layout_start->addWidget(pushbutton_start);
    h_layout_start->addWidget(pushbutton_stop);
    h_layout_start->addWidget(radiobutton_datataking);
    QObject::connect(pushbutton_start, &QPushButton::clicked, this, &SondePrincipale::StartAcquisition);
    QObject::connect(pushbutton_start, &QPushButton::clicked, this, [& ,radiobutton_datataking]() { radiobutton_datataking->setChecked(true); });
    QObject::connect(pushbutton_stop, &QPushButton::clicked, this, &SondePrincipale::StopAcquisition);
    QObject::connect(pushbutton_stop, &QPushButton::clicked, this, [&, radiobutton_datataking]() { radiobutton_datataking->setChecked(false); });

    QPushButton *pushbutton_multiply = new QPushButton("x10");
    QPushButton *pushbutton_divide = new QPushButton("/10");
    pushbutton_multiply->setMinimumHeight(40);
    pushbutton_divide->setMinimumHeight(40);
    QHBoxLayout *h_layout_multiply = new QHBoxLayout;
    h_layout_multiply->addWidget(pushbutton_multiply);
    h_layout_multiply->addWidget(pushbutton_divide);
    QObject::connect(pushbutton_multiply, &QPushButton::clicked, this, [&]() { factor_ = factor_ * 10; });
    QObject::connect(pushbutton_divide, &QPushButton::clicked, this, [&]() { factor_ = factor_ / 10; });

    QPushButton *pushbutton_save = new QPushButton("Save");
    pushbutton_save->setMinimumHeight(40);
    QObject::connect(pushbutton_save, &QPushButton::clicked, this, &SondePrincipale::Save);

    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(h_layout_plot);
    v_layout->addLayout(h_layout_start);
    v_layout->addLayout(h_layout_multiply);
    v_layout->addWidget(pushbutton_save);
    v_layout->addWidget(pushbutton_clear);

    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    timer->setSingleShot(false);
    timer->start();

    QObject::connect(timer, &QTimer::timeout, this, &SondePrincipale::Tick);
    QObject::connect(this, &SondePrincipale::SIGNAL_UpdateSeries, this, &SondePrincipale::UpdateSeries);
}

SondePrincipale::~SondePrincipale() {

}

void SondePrincipale::StartAcquisition() {
    acquisition_ = true;

}

void SondePrincipale::StopAcquisition() {
    acquisition_ = false;
}

void SondePrincipale::Save() {
    bool ok(false);
    QString text = QInputDialog::getText(this, tr("Please provide a name"),
                                             tr("Name:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
    if (ok) {
        SaveImage(text + ".png");
        SaveData(text + ".csv");
    }
}

void SondePrincipale::SaveImage(QString name) {
    this->grab().save(directory_ + name);
}

void SondePrincipale::SaveData(QString name) {
    if (!plot_.graph(0)) {
        qDebug() << "No graph";
    }
    QFile file(directory_ + name);
    if (file.open(QFile::WriteOnly|QFile::Truncate)) {
        QTextStream stream(&file);
        stream << "Position, Current\n";
        auto data = plot_.graph(0)->data();
        for (int idx = 0; idx < data->size(); ++idx) {
            stream << data->at(idx)->key << ", " << data->at(idx)->value << "\n";
        }
        file.close();
    }
}

void SondePrincipale::ClearGraph() {
    if (plot_.graph(0)) {
        plot_.graph(0)->data()->clear();
        plot_.replot();
    }
}

void SondePrincipale::SetTimeSeriesColors(QColor color) {
    if (plot_.graph(0)) {
        auto graph = plot_.graph(0);
        QColor linecolor(color);
        QPen linepen(linecolor);
        linepen.setWidth(2);
        graph->setPen(linepen);
        plot_.replot();
    } else {
        qWarning() << "No graph available";
    }
}

void SondePrincipale::SetupPlot() {
    plot_.setLocale(QLocale(QLocale::French, QLocale::Country::France));
    plot_.addGraph();
    SetTimeSeriesColors(0x209fdf);

    plot_.xAxis->setTickLabelColor(Qt::white);
    plot_.xAxis->setBasePen(QPen(Qt::white));
    plot_.xAxis->setLabelColor(Qt::white);
    plot_.xAxis->setTickPen(QPen(Qt::white));
    plot_.xAxis->setSubTickPen(QPen(Qt::white));
    plot_.yAxis->setTickLabelColor(Qt::white);
    plot_.yAxis->setBasePen(QPen(Qt::white));
    plot_.yAxis->setLabelColor(Qt::white);
    plot_.yAxis->setTickPen(QPen(Qt::white));
    plot_.yAxis->setSubTickPen(QPen(Qt::white));
    plot_.xAxis->setRange(0, 750);
    plot_.yAxis->setRange(-0.1, 11);

    plot_.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plot_.setBackground(QBrush(Qt::transparent));
    plot_.setStyleSheet("background-color: transparent;");
    plot_.replot();
}

void SondePrincipale::InterpretADCChannelRead(const QString& channel, double value, QString unit) {
    (void)unit;
    if (channel == "Libre 1") {
        current_ = value;
        current_set_ = true;
        emit SIGNAL_UpdateSeries();
    } else if (channel == "SP-D1") {
        position_ = value;
        position_set_ = true;
        emit SIGNAL_UpdateSeries();
    } else {
        // do nothing
    }
}

void SondePrincipale::Tick() {
    adc_.Read("Libre 1");
    adc_.Read("SP-D1");
}

void SondePrincipale::UpdateSeries() {
    qDebug() << "Pos, Cur " << position_ << " " << current_ << " scaled " << current_ * factor_;
//    if (!current_set_ || !position_set_ || !acquisition_) {
    if (!acquisition_) {
        return;
    }
    if (plot_.graph(0)) {
        auto graph = plot_.graph(0);
        graph->addData(position_, current_ * factor_);
        plot_.replot();
    } else {
        qWarning() << "BobinePrincipaleUI::UpdateTimeSeries No graph";
    }
}
