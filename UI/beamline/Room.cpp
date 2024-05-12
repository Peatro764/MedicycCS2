#include "Room.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QDebug>
#include <QSound>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Room::Room(QString title, double size, BeamItem item) : QGraphicsObject(), BeamItem(item),
    title_(title),
    label_(title, this),
    bounding_box_outer_(0, 0, size, size),
    bounding_box_inner_(2, 2, size - 4, size - 4)
{
    QFont font("Ubuntu");
    font.setPixelSize(20);
    label_.setFont(font);
    label_.setDefaultTextColor(Qt::yellow);
    label_.moveBy((bounding_box_outer_.width() - label_.boundingRect().width()) / 2, 1.1*bounding_box_outer_.height());
    update();
}

void Room::SetBeam(bool active, bool beam_delivered) {
    qDebug() << QDateTime::currentDateTime() << "Room " << title_ << " Active " << active
             << " beam_delivered " << beam_delivered << " beam_delivered_ " << beam_delivered_;
    bool beam_delivered_transition = (beam_delivered && !beam_delivered_);
    active_ = active;
    beam_delivered_ = beam_delivered;
    update();
    if (beam_delivered_transition) {
        qDebug() << QDateTime::currentDateTime() << " Play beam delivered";
        PlayBeamDelivered();
        emit SIGNAL_BeamDelivered();
    }
}

QRectF Room::boundingRect() const
{
    return bounding_box_outer_;
}

void Room::PlayBeamDelivered() {
    QSound::play(":/sounds/blaster.wav");
}

void Room::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    // Dont paint if no title given (notably M1)
    if (title_.isEmpty()) return;

    QColor color_inner("#26428B");
    QColor color_outer(Qt::white);

    if (active_) {
        color_inner = QColor("cornflowerblue");
        color_outer = QColor("cornflowerblue");
    }

    if (beam_delivered_) {
        color_inner = Qt::red;
        color_outer = Qt::red;
    }

    // Inner filled area
    color_inner.setAlpha(128);
    QBrush brush(color_inner);
    QPainterPath path_inner;
    path_inner.moveTo(bounding_box_inner_.width(), bounding_box_inner_.height() / 2);
    path_inner.arcTo(bounding_box_inner_, 0, 360.0*16);

    QPen pen_black(Qt::black);
    QBrush brush_black(Qt::black);
    pen_black.setBrush(brush_black);
    painter->setPen(pen_black);
    painter->setBrush(brush_black);
    painter->drawPath(path_inner);

    QPen pen_inner(color_inner);
    pen_inner.setWidth(2);
    pen_inner.setBrush(brush);
    painter->setPen(pen_inner);
    painter->setBrush(brush);
    painter->drawPath(path_inner);

    // Outer ring
    QPen pen_outer(color_outer);
    pen_outer.setWidth(4);
    painter->setPen(pen_outer);
    painter->drawEllipse(QPointF(bounding_box_outer_.width() / 2, bounding_box_outer_.height() / 2),
                         bounding_box_inner_.width() / 2,  bounding_box_inner_.width() / 2);
}

}
