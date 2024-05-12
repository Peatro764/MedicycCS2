#ifndef SLIDINGGRAPH_H
#define SLIDINGGRAPH_H

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QLogValueAxis>
#include <QGraphicsRectItem>

using namespace QtCharts;

namespace medicyc::cyclotroncontrolsystem::ui::electrometer {

class SlidingGraph : public QObject
{
Q_OBJECT
public:
    SlidingGraph(QtCharts::QChartView* chartview);
    ~SlidingGraph();
    void Clear();
    void SetTimeRange(double lower, double upper);
    void SetYRange(double lower, double upper);
    void SetUpperYRange(double upper);
    void SetLowerYRange(double lower);
    void AddMeasurement(double time, double value);

public slots:

private slots:
    void GraphPressed(const QPointF &point);

signals:
    void TimeIntervalGiven(double start_time, double end_time);

private:
    QChartView* chartview_;
    QLineSeries data_;
    QDateTimeAxis axis_x_;
    QLogValueAxis axis_y_;
};

} // ns

#endif
