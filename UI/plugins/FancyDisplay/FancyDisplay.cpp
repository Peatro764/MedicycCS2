#include "FancyDisplay.h"

#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

FancyDisplay::FancyDisplay(QWidget *parent, GeometricShape shape, QColor color, int width, int height, double angle, QString unit)
    : QWidget(parent), shape_(shape),
      width_(width), height_(height), angle_(angle),
      unit_(unit)
{
    setFixedSize(width, height);
    color_change_timer_.setSingleShot(true);
    color_change_timer_.setInterval(100);
    QObject::connect(this, &FancyDisplay::SIGNAL_Clicked, this, [&]() {
        SetClickedColor();
        color_change_timer_.start();
    });
    QObject::connect(&color_change_timer_, &QTimer::timeout, this, &FancyDisplay::SetDefaultColor);

//    QRadialGradient radialGrad(height/2, width/2, 200);
     QRadialGradient radialGrad(width / 2, height / 2, (width + height) / 2);
    radialGrad.setColorAt(0, color);
    radialGrad.setColorAt(1, Qt::black);
    standard_brush_ = QBrush(radialGrad);
    active_brush_ = standard_brush_;
}

void FancyDisplay::SetValue(double value) {
    if (std::abs(display_value_ - value) > 0.00001) {
        emit SIGNAL_ValueChanged();
    }
    display_value_ = value;
    update();
}

void FancyDisplay::SetDefaultColor() {
    active_brush_ = standard_brush_;
    update();
}

void FancyDisplay::SetClickedColor() {
    active_brush_ = clicked_brush_;
    update();
}

void FancyDisplay::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    switch(shape_) {
    case GeometricShape::ELLIPS:
        DrawEllips(&painter);
    }
}

void FancyDisplay::DrawEllips(QPainter *painter) {
    const int w = width() / 1.1;
    const int h = height() / 1.1;
    const int x_left_corner = (width() - w ) / 2;
    const int y_upper_corner = (height() - h ) / 2;
    QRectF rect(x_left_corner, y_upper_corner, w, h);

    painter->setBrush(active_brush_);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(Qt::white);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    painter->drawEllipse(rect);

    QFont value_font = QFont("Sans Serif", 18);
    painter->setFont(value_font);
    painter->setPen(QPen(Qt::white));
    painter->drawText(rect, Qt::AlignCenter, QString::number(display_value_, 'f', 3) + " " + unit_);
}

void FancyDisplay::mousePressEvent ( QMouseEvent * event )
{
    (void)event;
    qDebug() << "FancyDisplay::mousePressEvent";
    emit SIGNAL_Clicked();
}
