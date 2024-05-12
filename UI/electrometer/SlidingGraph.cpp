#include "SlidingGraph.h"

#include <QDebug>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::ui::electrometer {

SlidingGraph::SlidingGraph(QtCharts::QChartView *chartview)
    : chartview_(chartview) {

    data_.setName("Current");

    //chartview->setRubberBand(QChartView::RectangleRubberBand);

    QPen rad_pen;
    rad_pen.setColor(QRgb(0x209fdf));
    rad_pen.setWidth(3);
    data_.setPen(rad_pen);

    auto chart = chartview_->chart();
    QColor bg_color = QColor("#31363b");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setBackgroundBrush(QBrush(bg_color));
    chart->legend()->setVisible(false);
    chartview->setRenderHint(QPainter::Antialiasing);
    chart->setDropShadowEnabled(false);

    axis_x_.setTickCount(3);
    axis_x_.setFormat("mm:ss");

    axis_y_.setLabelsFont(QFont("Arial", 8));
    axis_y_.setTitleText("Courant [A]");
    axis_y_.setTitleFont(QFont("Arial", 10));
    axis_y_.setLabelFormat(QString("%2.1E"));

    axis_y_.setGridLineVisible(true);

    axis_x_.setTitleBrush(QBrush(Qt::white));
    axis_x_.setLabelsBrush(QBrush(Qt::white));
    axis_x_.setShadesBrush(QBrush(Qt::white));
    axis_y_.setTitleBrush(QBrush(Qt::white));
    axis_y_.setLabelsBrush(QBrush(Qt::white));
    axis_y_.setShadesBrush(QBrush(Qt::white));

    chart->addSeries(&data_);
    chart->addSeries(&data_);

    chart->addAxis(&axis_x_, Qt::AlignBottom);
    data_.attachAxis(&axis_x_);

    chart->addAxis(&axis_y_, Qt::AlignLeft);
    data_.attachAxis(&axis_y_);

    axis_x_.setRange(QDateTime::fromMSecsSinceEpoch(0), QDateTime::fromMSecsSinceEpoch(10));
    axis_y_.setRange(1e-13, 1e-12);
    axis_y_.setBase(10);

    chartview_->repaint();

    QObject::connect(&data_, &QLineSeries::pressed, this, &SlidingGraph::GraphPressed);
}

void SlidingGraph::GraphPressed(const QPointF &point) {
    /*
    if (manual_measurement_pos_set_) {
        manual_measurement_pos_set_ = false;
        double stop_time = static_cast<double>(point.x());
        emit(TimeIntervalGiven(manual_measurement_pos_, stop_time));
    } else {
        manual_measurement_pos_ = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(point.x()));
        manual_measurement_pos_set_ = true;
    }
    */
}

SlidingGraph::~SlidingGraph()  {
    auto chart = chartview_->chart();
    chart->removeSeries(&data_);
    chart->removeAxis(&axis_x_);
    chart->removeAxis(&axis_y_);
}

void SlidingGraph::SetTimeRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "SlidingGraph::SetTimeRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_x_.setRange(QDateTime::fromMSecsSinceEpoch(1000*lower), QDateTime::fromMSecsSinceEpoch(1000*upper));
}

void SlidingGraph::SetLowerYRange(double lower) {
    axis_y_.setMin(lower);
}

void SlidingGraph::SetUpperYRange(double upper) {
    axis_y_.setMax(upper);
}

void SlidingGraph::SetYRange(double lower, double upper) {
    if (upper <= lower) {
        qWarning() << "SlidingGraph::SetYRange Upper limit equal or smaller than lower limit";
        return;
    }
    axis_y_.setRange(lower, upper);
}

void SlidingGraph::AddMeasurement(double time, double value) {
    const double abs_value = abs(value);
    double time_ms = time*1000;
    axis_x_.setRange(QDateTime::fromMSecsSinceEpoch(time_ms - 10*60*1000), QDateTime::fromMSecsSinceEpoch(time_ms));
    data_.append(time_ms, abs_value);

    if (abs_value < axis_y_.min()) {
        axis_y_.setRange(abs_value * 0.75, axis_y_.max());
    }
    if (abs_value > axis_y_.max()) {
        axis_y_.setRange(axis_y_.min(), abs_value*1.25);
    }

    if (data_.at(0).x() < axis_x_.min().toMSecsSinceEpoch()) {
        data_.remove(0);
    }
}

void SlidingGraph::Clear() {
    data_.clear();
}

}
