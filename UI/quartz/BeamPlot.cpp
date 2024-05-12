#include <qwt_color_map.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>
#include <qwt_scale_map.h>
#include <qwt_symbol.h>
#include <QPen>
#include <QDebug>

#include "BeamPlot.h"
#include "SpectrogramData.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap(Qt::black, Qt::red) {
        addColorStop(0.2, Qt::blue);
        addColorStop(0.5, Qt::yellow);
        addColorStop(0.75, QColor(255, 175, 0));
    }
};

BeamPlot::BeamPlot(QString name, QWidget *parent ):
    QwtPlot( parent ), name_(name)
{
    qDebug() << "BeamPlot::BeamPlot " << name;
    spectrogram_ = new QwtPlotSpectrogram();
    spectrogram_->setRenderThreadCount( 0 ); // use system specific thread count
    spectrogram_->setColorMap( new ColorMap() );
    spectrogram_->attach( this );

    grid_ = new QwtPlotGrid();
    grid_->enableXMin( true );
    grid_->enableYMin( true );
    grid_->setMajorPen( QPen( Qt::gray, 0, Qt::SolidLine ) );
    grid_->setMinorPen( QPen( Qt::gray, 0 , Qt::DotLine ) );

    QList<double> contourLevels;
//    for ( double level = (Z_MAX / 10); level < Z_MAX; level += (Z_MAX / 10) )
//        contourLevels += level;
    contourLevels += 98.0;
    contourLevels += 50.0;
    spectrogram_->setContourLevels( contourLevels );

    const QwtInterval zInterval(Z_MIN, Z_MAX);
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap( zInterval, new ColorMap() );
    rightAxis->setAttribute(Qt::WA_TranslucentBackground);
    rightAxis->setAutoFillBackground(false);
    rightAxis->setAttribute(Qt::WA_NoSystemBackground);
    rightAxis->setAttribute(Qt::WA_TranslucentBackground);
    setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    enableAxis(QwtPlot::yRight);
    plotLayout()->setAlignCanvasToScales( true );

    zoomer_ = new BeamZoomer( canvas(), &data_ );
    zoomer_->setZoomBase(true);
    QObject::connect(zoomer_, &QwtPlotZoomer::zoomed, this, &BeamPlot::Zoomed);

    QwtPlotPicker *picker = new QwtPlotPicker(canvas());
    picker->setStateMachine(new QwtPickerClickPointMachine());
    QObject::connect(picker, qOverload<const QPointF&>(&QwtPlotPicker::selected), this, &BeamPlot::SIGNAL_MouseClick);
    QObject::connect(picker, qOverload<const QPointF&>(&QwtPlotPicker::selected), this, [&](const QPointF& pos) {
        QwtScaleMap x_map = this->canvasMap(QwtPlot::xBottom);
        QwtScaleMap y_map = canvasMap(QwtAxis::YLeft);
    });

    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
    sd->setMinimumExtent( fm.width( "100.00" ) );
    QVector<double> data {0, 0, 0, 0};
    SetData(data, 2);

    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);
    canvas()->setAutoFillBackground( false );
    setStyleSheet("Background: transparent");

    replot();
}

void BeamPlot::Zoomed(const QRectF& rect) {
    qDebug() << "BeamPlot::Zoomed " << rect;
    zoom_ = rect;
}

void BeamPlot::SetData(QVector<double> data, int ncol) {
//    qDebug() << "BeamPlot::SetData";
    data_.SetInterval(x_int_,
                      y_int_,
                      QwtInterval(Z_MIN, Z_MAX));
    data_.SetData(data, ncol);
    spectrogram_->setData(&data_);
    zoomer_->setZoomBase(QRectF(x_int_.minValue(), y_int_.minValue(), x_int_.width(), y_int_.width()));
//    DrawMaxPos(data_.GetMaxPosition()); // draws a cross on the max pos
    replot();
}

void BeamPlot::SetImage(cv::Mat& frame) {
//    qDebug() << "BeamPlot::SetImage";
    frame_ = frame;
    data_.SetInterval(x_int_,
                      y_int_,
                      QwtInterval(Z_MIN, Z_MAX));
    data_.SetData(frame);
    spectrogram_->setData(&data_);
    zoomer_->setZoomBase(QRectF(x_int_.minValue(), y_int_.minValue(), x_int_.width(), y_int_.width()));
//    DrawMaxPos(data_.GetMaxPosition());
    replot();
}

void BeamPlot::SetInterval(QwtInterval x, QwtInterval y) {
//    qDebug() << "BeamPlot::SetInterval ";
    x_int_ = x;
    y_int_ = y;
    data_.SetInterval(x_int_,
                      y_int_,
                      QwtInterval(Z_MIN, Z_MAX));
    setAxisScale(QwtPlot::yLeft, y_int_.minValue(), y_int_.maxValue());
    setAxisScale(QwtPlot::xBottom, x_int_.minValue(), x_int_.maxValue());
    replot();
}

void BeamPlot::GetInterval(QwtInterval& x, QwtInterval& y) const {
    qDebug() << "BeamPlot::GetInterval";
    x = x_int_;
    y = y_int_;
}

void BeamPlot::DrawGrid(bool on) {
    qDebug() << "BeamPlot::DrawGrid " << on;
    if (on) {
        grid_->attach( this );
    } else {
        grid_->detach();
    }
    replot();
}

void BeamPlot::DrawMaxPos(QPointF pos) {
//    qDebug() << "BeamPlot::DrawMaxPos";
    if (marker_) {
        marker_->detach();
        delete marker_;
    }
    marker_ = new QwtPlotMarker();
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(5);
    marker_->setSymbol(new QwtSymbol(QwtSymbol::Cross, Qt::magenta, pen, QSize( 30, 30 )));
    marker_->setValue(pos);
    marker_->attach( this );
}

void BeamPlot::ShowContour( bool on )
{
    qDebug() << "BeamPlot::ShowContour " << on;
    spectrogram_->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    replot();
}

void BeamPlot::ShowSpectrogram( bool on )
{
    qDebug() << "BeamPlot::ShowSpectrogram " << on;
    spectrogram_->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    spectrogram_->setDefaultContourPen( on ? QPen() : QPen( Qt::NoPen ) );
    replot();
}

}
