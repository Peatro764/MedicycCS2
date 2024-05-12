#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "qcustomplot.h"

#include <vector>
#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

class Histogram : public QWidget {

public:
    Histogram(QWidget* parent = 0, double min = 0, double max = 100, int n_bins = 100);
    ~Histogram();
    void SetBins(int nbins, double min, double max);
    void AddData(double value, double weight);
    void SetData(QVector<double> values);
    void Clear();
    void RemoveAll();
    void Normalize();
    void SetXLabel(QString label);
    void ShowLegend();
    double Mean() const;
    double StdDev() const;

public slots:
    void mousePress();
    void mouseWheel();

private:
    void SetupWidget();
    void GenerateColors();
    void Plot();
    double Max(const QVector<double>& data) const;
    double BinSize() { return (max_ - min_) / static_cast<double>(n_bins_); }
    QColor GetColor() const;
    void AddValue(double value, double weight);
    void InitDataVectors();
    QCustomPlot customplot_;
    QCPBars bars_;
    QVector<double> keys_;
    QVector<double> values_;
    int n_bins_;
    double min_;
    double max_;

    std::vector<QColor> colors_;
};

#endif

