#ifndef BEAMPLOT_H
#define BEAMPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <QImage>
#include <opencv2/imgproc/imgproc.hpp>
#include <qwt_plot_marker.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_zoomer.h>
#include <qwt_text.h>

#include "SpectrogramData.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class BeamZoomer: public QwtPlotZoomer
{
public:
    BeamZoomer( QWidget *canvas, SpectrogramData* data ):
        QwtPlotZoomer( canvas ), data_(data)
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        double z_value = data_->value(pos.x(), pos.y());
        QString text = QString("(x,y) = (%1,%2) z = %3").arg(pos.x(), 0, 'g', 3).arg(pos.y(), 0, 'g', 3).arg(z_value, 0, 'g', 3);
        QwtText qwtText(text);
        qwtText.setBackgroundBrush( QBrush( bg ) );
        return qwtText;
    }
private:
    SpectrogramData *data_;
};


class BeamPlot: public QwtPlot
{
    Q_OBJECT

public:
    BeamPlot(QString name, QWidget * = NULL);
    void SetName(QString name) { name_ = name; }
    QString GetName() const { return name_; }
    void SetImage(cv::Mat& frame);
    cv::Mat GetImage() const { return frame_; }
    void SetInterval(QwtInterval x, QwtInterval y);
    void GetInterval(QwtInterval& x, QwtInterval& y) const;
    void SetData(QVector<double> data, int ncol);
    void GetData(QVector<double>& data, int& ncol) const { data_.GetData(data, ncol); }
    double GetMaxValue() const { return data_.GetMaxValue(); }
    QPointF GetMaxPosition() const { return data_.GetMaxPosition(); }
    QRectF GetZoom() const { return zoom_; }
    void ApplyZoom(QRectF rect) { zoomer_->zoom(rect); }

public slots:
    void ShowContour( bool on );
    void ShowSpectrogram( bool on );
    void DrawGrid(bool on);
    void DrawMaxPos(QPointF pos);

private slots:
    void Zoomed(const QRectF& rect);

signals:
    void SIGNAL_MouseClick(const QPointF& pos);

private:
    QString name_;
    cv::Mat frame_;
    QwtPlotSpectrogram *spectrogram_ = nullptr;
    SpectrogramData data_;
    QwtPlotMarker* marker_ = nullptr;
    BeamZoomer* zoomer_ = nullptr;
    QwtPlotGrid* grid_ = nullptr;
    const double Z_MIN = 0;
    const double Z_MAX = 100;
    QwtInterval x_int_ = QwtInterval(-20, 20);
    QwtInterval y_int_ = QwtInterval(-20, 20);
    QRectF zoom_ = QRectF(-20, -20, 40, 40);
};

} // ns

#endif
