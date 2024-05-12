#include "Histogram.h"

#include <QVector>
#include <algorithm>

Histogram::Histogram(QWidget* parent, double min, double max, int n_bins)
    : QWidget(parent),
      bars_(customplot_.xAxis, customplot_.yAxis),
      n_bins_(n_bins),
      min_(min),
      max_(max)
{
    GenerateColors();
    InitDataVectors();
    SetupWidget();
    QColor fg_color = Qt::white;
    QColor bg_color = QColor("#31363b");

    //        customplot_.axisRect()->setupFullAxesBox();
    customplot_.setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));
    customplot_.setBackground(bg_color);
    customplot_.legend->setBrush(QBrush(bg_color));
    customplot_.legend->setBorderPen(QPen(fg_color));
    customplot_.legend->setTextColor(fg_color);
    customplot_.yAxis->setLabelColor(fg_color);
    customplot_.yAxis->setBasePen(QPen(fg_color, 1));
    customplot_.yAxis->setTickPen(QPen(fg_color, 1));
    customplot_.yAxis->setSubTickPen(QPen(fg_color, 1));
    customplot_.yAxis->setTickLabelColor(fg_color);
    customplot_.yAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    customplot_.yAxis->grid()->setSubGridVisible(false);

    customplot_.xAxis->setLabelColor(fg_color);
    customplot_.xAxis->setBasePen(QPen(fg_color, 1));
    customplot_.xAxis->setTickPen(QPen(fg_color, 1));
    customplot_.xAxis->setSubTickPen(QPen(fg_color, 1));
    customplot_.xAxis->setTickLabelColor(fg_color);
    customplot_.xAxis->grid()->setPen(QPen(fg_color, 0.5, Qt::DotLine));
    customplot_.xAxis->grid()->setSubGridVisible(false);
    customplot_.xAxis->setRange(0.0, 10.0);

    customplot_.yAxis->setLabel(QString(""));
    customplot_.yAxis->setTickLabels(false);
    customplot_.xAxis->setRange(min_, max_);
    customplot_.yAxis->setRange(0.0, 10.0);
    customplot_.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom); // | QCP::iSelectItems);
    QObject::connect(&customplot_, &QCustomPlot::mousePress, this, &Histogram::mousePress);
    QObject::connect(&customplot_, &QCustomPlot::mouseWheel, this, &Histogram::mouseWheel);

    QColor color = GetColor();
    QPen pen(color);
    pen.setWidth(3);
    color.setAlpha(122);
    QBrush brush(color);
    bars_.setPen(pen);
    bars_.setName("NONAME");
    bars_.setAntialiased(false);
    bars_.setAntialiasedFill(false);
    bars_.setBrush(brush); //QColor("#FFA100"));
    bars_.setVisible(true);
    bars_.setWidth(BinSize());
    bars_.setData(keys_, values_);
}


Histogram::~Histogram() {
    customplot_.clearGraphs();
}

void Histogram::SetXLabel(QString label) {
    customplot_.xAxis->setLabel(label);
}

void Histogram::InitDataVectors() {
    keys_.clear();
    values_.clear();
    for (int i = 0; i < n_bins_; ++i) {
        keys_.push_back(min_ + 0.5 * BinSize() + (static_cast<double>(i) * BinSize()));
    }
    values_ = QVector<double>(n_bins_);
}

void Histogram::SetupWidget() {
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(&customplot_);
    this->setLayout(layout);
}

void Histogram::ShowLegend() {
    customplot_.legend->setFont(QFont("Helvetica",7));
    customplot_.legend->setVisible(true);
    customplot_.update();
    customplot_.replot();
}

void Histogram::GenerateColors() {
    colors_.push_back(QRgb(0x209fdf));
    colors_.push_back(QRgb(0x99ca53));
    colors_.push_back(QRgb(0xf6a625));
    colors_.push_back(QRgb(0x6d5fd5));
    colors_.push_back(QRgb(0xbf593e));
}

QColor Histogram::GetColor() const {
    return colors_.at(0);
}

double Histogram::Mean() const {
//    return calc::Mean(values_);
    return 0;
}

double Histogram::StdDev() const {
//    return calc::StdDev(values_);
    return 0;
}

void Histogram::SetBins(int nbins, double min, double max) {
    assert(nbins > 0);
    n_bins_ = nbins;
    min_ = min;
    max_ = max;
    customplot_.xAxis->setRange(min, max);
    InitDataVectors();
    Plot();
}

void Histogram::SetData(QVector<double> values) {
    values_ = values;
    bars_.setData(keys_, values_);
    Plot();
}

void Histogram::AddData(double value, double weight) {
    int bin = static_cast<int>((value - min_) / BinSize());
    if (bin < 0) bin = 0;
    if (bin >= n_bins_) bin = (n_bins_ - 1);
    values_[bin] += weight;

    bars_.setData(keys_, values_);
    Plot();
}

void Histogram::RemoveAll() {
    customplot_.clearGraphs();
    values_.clear();
    keys_.clear();
}

void Histogram::Clear() {
    InitDataVectors();
    bars_.setData(keys_, values_);
    Plot();
}

void Histogram::Plot() {
    const double max = Max(values_);
    customplot_.yAxis->setRangeUpper(max * 1.2);
    customplot_.replot();
}

double Histogram::Max(const QVector<double>& data) const {
 return *std::max_element(data.constBegin(), data.constEnd());
}

void Histogram::Normalize() {
    const double max = Max(values_);
    for (double &value : values_) {
        value = value / max;
    }
    bars_.setData(keys_, values_);
    customplot_.yAxis->setRangeUpper(1.1);
    customplot_.replot();
}

void Histogram::mousePress() {
    customplot_.axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void Histogram::mouseWheel() {
    customplot_.axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
