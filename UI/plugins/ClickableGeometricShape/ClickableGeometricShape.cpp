#include "ClickableGeometricShape.h"

#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

ClickableGeometricShape::ClickableGeometricShape(QWidget *parent, GeometricShape shape, QBrush brush, int width, int height, double angle)
    : QWidget(parent), shape_(shape), standard_brush_(brush), active_brush_(brush), width_(width), height_(height), angle_(angle)
{
    setFixedSize(width, height);
    color_change_timer_.setSingleShot(true);
    color_change_timer_.setInterval(100);
    QObject::connect(this, &ClickableGeometricShape::SIGNAL_Clicked, this, [&]() {
        SetClickedColor();
        color_change_timer_.start();
    });
    QObject::connect(&color_change_timer_, &QTimer::timeout, this, &ClickableGeometricShape::SetDefaultColor);
}

void ClickableGeometricShape::SetDefaultColor() {
    active_brush_ = standard_brush_;
    update();
}

void ClickableGeometricShape::SetClickedColor() {
    active_brush_ = clicked_brush_;
    update();
}

void ClickableGeometricShape::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    switch(shape_) {
    case GeometricShape::DOUBLEARROW:
        DrawDoubleArrow(&painter);
    }
}

void ClickableGeometricShape::DrawDoubleArrow(QPainter *painter) {
    const int w = width();
    const int h = height();
    painter->setBrush(active_brush_);

    const QPointF points[3] = {
        QPointF(0.0 - w/2, 0.0 - h/2),
        QPointF(w / 2 - w/2, h - h/2),
        QPointF(w - w/2, 0 - h/2)
    };
    painter->translate(w/2, h/2);
    painter->rotate(angle_);
    painter->drawPolygon(points, 3);
}

void ClickableGeometricShape::mousePressEvent ( QMouseEvent * event )
{
    (void)event;
    qDebug() << "ClickableGeometricShape::mousePressEvent";
    emit SIGNAL_Clicked();
}
