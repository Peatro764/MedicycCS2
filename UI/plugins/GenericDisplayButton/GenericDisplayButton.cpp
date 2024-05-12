#include "GenericDisplayButton.h"

#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

GenericDisplayButton::GenericDisplayButton(QWidget *parent, int side)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(side, side);
    Enable();
}

void GenericDisplayButton::SetupAsCenterFilling(QColor color) {
    color_changing_button_ = false;
    color_on_ = color;
    color_off_ = color;
}

void GenericDisplayButton::SetupAsColorChanging(QColor on, QColor off) {
    color_changing_button_ = true;
    color_on_ = on;
    color_off_ = off;
}

void GenericDisplayButton::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor color = COLOR_DISABLED;
    if (enabled_) {
        if (color_changing_button_) {
            color = on_ ? color_on_ : color_off_;
        } else {
            color = color_on_;
        }
    }

    DrawArc(&painter, color);
    if (color_changing_button_ || on_) {
       DrawCenter(&painter, color);
    }
}

void GenericDisplayButton::SetEnable(bool flag) {
    enabled_ = flag;
    update();
}

void GenericDisplayButton::On() {
    on_ = true;
    update();
}

void GenericDisplayButton::Off() {
    on_ = false;
    update();
}

QString GenericDisplayButton::text() const {
    return (on_ ? QString("ON") : QString("OFF"));
}

void GenericDisplayButton::Enable() {
    SetEnable(true);
}

void GenericDisplayButton::Disable() {
    SetEnable(false);
}

void GenericDisplayButton::DrawCenter(QPainter *painter, QColor color) {
    const int w = width() / 2;
    const int h = height() / 2;
    const int x_left_corner = (width() - w ) / 2;
    const int y_upper_corner = (height() - h ) / 2;
    QRectF rect(x_left_corner, y_upper_corner, w, h);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    pen.setBrush(color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    painter->setBrush(brush);
    painter->drawEllipse(rect);
}

void GenericDisplayButton::DrawArc(QPainter *painter, QColor color) {
    const int w = width() / 1.2;// / 1.75;
    const int h = height() / 1.2; // / 1.75;
    const int x_left_corner = (width() - w ) / 2;
    const int y_upper_corner = (height() - h ) / 2;
    QRectF rect(x_left_corner, y_upper_corner, w, h);

    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setBrush(color);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawArc(rect, 0, 360 * 16);
}

