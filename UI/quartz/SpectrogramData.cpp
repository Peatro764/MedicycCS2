#include "SpectrogramData.h"

#include <QDebug>
#include <QImage>
#include <cstring>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

SpectrogramData::SpectrogramData() {
    qDebug() << "SpectroGramData::SpectrogramData";
    this->setResampleMode(ResampleMode::BilinearInterpolation);
}

void SpectrogramData::SetInterval(QwtInterval x_range, QwtInterval y_range, QwtInterval z_range) {
//    qDebug() << "SpectroGramData::SetInterval " << x_range << " " << y_range << " " << z_range;
    setInterval( Qt::XAxis, x_range );
    setInterval( Qt::YAxis, y_range );
    setInterval( Qt::ZAxis, z_range );
}

void SpectrogramData::GetInterval(QwtInterval& x_range, QwtInterval& y_range, QwtInterval& z_range) const {
//    qDebug() << "SpectroGramData::GetInterval";
    x_range = interval(Qt::XAxis);
    y_range = interval(Qt::YAxis);
    z_range = interval(Qt::ZAxis);
}

void SpectrogramData::SetData(cv::Mat& frame) {
    if (frame.empty()) {
        qWarning() << "SpectrogramData::SetData frame is empty, returning";
        return;
    }
    QVector<uchar> array_c(frame.rows*frame.cols);
    QVector<double> array_d(frame.rows*frame.cols);
    std::memcpy(array_c.data(), frame.data, frame.rows*frame.cols);
    std::copy(std::begin(array_c), std::end(array_c), std::begin(array_d));

    QVector<double>::iterator it_max = std::max_element(array_d.begin(), array_d.end());
    int max_index = (it_max - array_d.begin());
    max_value_ = *it_max;
    int max_index_x = max_index % frame.cols;
    int max_index_y = max_index / frame.cols;
    pos_max_value_ = QPointF((static_cast<double>(max_index_x) / static_cast<double>(frame.cols)) * interval(Qt::XAxis).width() + interval(Qt::XAxis).minValue(),
                             (static_cast<double>(max_index_y) / static_cast<double>(frame.rows)) * interval(Qt::YAxis).width() + interval(Qt::YAxis).minValue());


    for (int i = 0; i < array_d.size(); ++i) {
        array_d[i] = array_d[i] / max_value_ * 100;
    }
    setValueMatrix(array_d, frame.cols);
}

void SpectrogramData::SetData(QVector<double> data, int ncol) {
//    qDebug() << "SpectroGramData::SetData";
    setValueMatrix(data, ncol);
}

void SpectrogramData::GetData(QVector<double>& data, int& ncol) const {
//    qDebug() << "SpectroGramData::GetData";
    data = valueMatrix();
    ncol = numColumns();
}


}
