#include "Cyclotron.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Cyclotron::Cyclotron(QString title, double size, double beam_pipe_angle, double beam_pipe_delta_angle) : QGraphicsObject(),
    beam_pipe_angle_(beam_pipe_angle),
    beam_pipe_delta_angle_(beam_pipe_delta_angle),
    label_(title, this),
    bounding_box_outer_(0, 0, size, size),
    bounding_box_inner_(4, 4, size - 8, size - 8)
{
    QFont font("Ubuntu");
    font.setPixelSize(20);
    label_.setFont(font);
    label_.setDefaultTextColor(Qt::yellow);
    label_.moveBy((bounding_box_outer_.width() - label_.boundingRect().width()) / 2 + 20, 1.1*bounding_box_outer_.height());
    SetBeam(false);
}

void Cyclotron::SetBeam(bool on) {
    if (on) {
        color_inner_ = Qt::red;
    } else {
        color_inner_ = QColor("cornflowerblue");
    }
    update();
}

QRectF Cyclotron::boundingRect() const
{
    return bounding_box_outer_;
}

void Cyclotron::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QColor color_outer("#DBE2E9");

    QBrush brush_inner(color_inner_);

    QPainterPath path_outer;
    path_outer.moveTo(bounding_box_outer_.width(), bounding_box_outer_.height() / 2);
    path_outer.arcTo(bounding_box_outer_, 0, 360.0*16);

    QPainterPath path_inner;
    path_inner.moveTo(bounding_box_inner_.width(), bounding_box_inner_.height() / 2);
    path_inner.arcTo(bounding_box_inner_, 0, 360.0*16);

    QPen pen_outer(color_outer);
    pen_outer.setWidth(6);
    painter->setPen(pen_outer);
    painter->drawArc(bounding_box_outer_, 16*180/3.1415*(beam_pipe_angle_ - beam_pipe_delta_angle_ / 2), -16*(360 - 180/3.1415*beam_pipe_delta_angle_));

    QPen pen_black(Qt::black);
    QBrush brush_black(Qt::black);
    pen_black.setBrush(brush_black);
    painter->setPen(pen_black);
    painter->setBrush(brush_black);
    painter->drawPath(path_inner);

    QPen pen_inner(color_inner_);
    pen_inner.setWidth(2);
    pen_inner.setBrush(brush_inner);
    painter->setPen(pen_inner);
    painter->setBrush(brush_inner);
    painter->drawPath(path_inner);
}

}
