#ifndef SONDEPRINCIPALE_H
#define SONDEPRINCIPALE_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>

#include "qcustomplot.h"
#include "adc_interface.h"

class SondePrincipale : public QWidget
{
    Q_OBJECT
public:
    SondePrincipale(QWidget *parent = 0);
    ~SondePrincipale();

public slots:

signals:
    void SIGNAL_UpdateSeries();

private slots:
    void Tick();
    void UpdateSeries();
    void InterpretADCChannelRead(const QString& channel, double value, QString unit);
    void ClearGraph();
    void Save();
    void SaveImage(QString name);
    void SaveData(QString name);
    void StartAcquisition();
    void StopAcquisition();

private:
    void SetupPlot();
    void SetTimeSeriesColors(QColor color);

    QCustomPlot plot_;
    medicyc::cyclotron::ADCInterface adc_;
    double current_ = 0.0;
    double position_ = 0.0;
    bool current_set_ = false;
    bool position_set_ = false;
    double factor_ = 1;
    bool acquisition_ = false;
    QString directory_ = "/home/hofverberg/Documents/ISO/Measurements/";
};

#endif
