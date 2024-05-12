#ifndef SPECTROGRAMDATA_H
#define SPECTROGRAMDATA_H

#include <qwt_matrix_raster_data.h>
#include <qwt_raster_data.h>
#include <qwt_interval.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <QPointF>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class SpectrogramData: public QwtMatrixRasterData
{
public:
    SpectrogramData();
    void SetInterval(QwtInterval x_range, QwtInterval y_range, QwtInterval z_range);
    void GetInterval(QwtInterval& x_range, QwtInterval& y_range, QwtInterval& z_range) const;
    void SetData(cv::Mat& frame);
    void SetData(QVector<double> data, int ncol);
    void GetData(QVector<double>& data, int& ncol) const;
    double GetMaxValue() const { return max_value_; }
    QPointF GetMaxPosition() const { return pos_max_value_; }

private:
    double max_value_;
    QPointF pos_max_value_;
};

}

#endif

