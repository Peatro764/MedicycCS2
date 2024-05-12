#include "Quadrupole.h"

#include <QColor>
#include <QBrush>
#include <QPainter>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Quadrupole::Quadrupole(QString title, BeamItem item) : QGraphicsObject(), BeamItem(item),
    rect_(0, 0, 1.2*Constants::PIPE_OUTER_RADIUS, 3*Constants::PIPE_OUTER_RADIUS),
    label_(title, this)
{
    QFont font("Ubuntu");
    font.setPixelSize(Constants::BEAM_ITEM_FONT_SIZE);
    label_.moveBy(-(label_.boundingRect().width() - rect_.width()), -2*Constants::PIPE_OUTER_RADIUS);
    label_.setFont(font);
    label_.setDefaultTextColor(brown_);
    update();
}

QRectF Quadrupole::boundingRect() const
{
    return rect_;
}

void Quadrupole::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QColor brown_transparent(brown_);
    brown_transparent.setAlpha(128);

    QPen pen(brown_);
    pen.setBrush(brown_transparent);
    pen.setWidth(2);

    painter->setPen(pen);
    painter->setBrush(brown_transparent);
    painter->drawRect(rect_);

    painter->drawLine(rect_.topLeft() + QPoint(0, rect_.height() / 2),
                      rect_.bottomRight() - QPoint(0, rect_.height() / 2));
}

}
