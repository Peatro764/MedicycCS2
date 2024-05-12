#include "Dipole.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <math.h>
#include <QDebug>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Dipole::Dipole(QString name, double span_angle, BeamItem item) : QGraphicsObject(), BeamItem(item),
    name_(name),
    span_angle_(span_angle),
    label_(name, this)
{
    QFont font("Ubuntu");
    font.setPixelSize(Constants::BEAM_ITEM_FONT_SIZE);
    label_.moveBy(-Constants::PIPE_OUTER_RADIUS, -3*Constants::PIPE_OUTER_RADIUS);
    label_.setFont(font);
    label_.setDefaultTextColor(brown_);

    Build();
    update();
}

void Dipole::Build() {
    p0 = QPoint(0, 0);
    p1 = QPoint(-HALF_LENGTH_, 0);
    p2 = QPoint(HALF_LENGTH_*std::cos(span_angle_), HALF_LENGTH_*std::sin(span_angle_));

    const double hyp = HALF_LENGTH_ * std::sqrt(2 + 2*std::cos(span_angle_));
    const double bending_radius = hyp / 2.0 / std::sin(span_angle_/2);
    double dipole_halfwidth = 1.5 * Constants::PIPE_OUTER_RADIUS;
    A1 = QPoint(p1.x(), p1.y() - dipole_halfwidth);
    A2 = QPoint(p1.x(), p1.y() + dipole_halfwidth);
    B1 = QPoint(p2.x() + dipole_halfwidth*std::cos(90.0*3.1415/180.0 - span_angle_),
                       p2.y() - dipole_halfwidth*std::sin(90.0*3.1415/180.0 - span_angle_));
    B2 = QPoint(p2.x() - dipole_halfwidth*cos(90.0*3.1415/180.0 - span_angle_),
                       p2.y() + dipole_halfwidth*sin(90.0*3.1415/180.0 - span_angle_));
    rect_outer_ = QRect(A1.x() - (bending_radius + dipole_halfwidth), A1.y(),
                     2 * (bending_radius + dipole_halfwidth),
                     2 * (bending_radius + dipole_halfwidth));
    rect_inner_ = QRect(A2.x() - (bending_radius - dipole_halfwidth), A2.y(),
                     2 * (bending_radius - dipole_halfwidth),
                     2 * (bending_radius - dipole_halfwidth));
    path_.clear();
    path_.moveTo(A1);
    path_.lineTo(A2);
    // arcTo expects degrees and not radius like all other functions...
    path_.arcTo(rect_inner_, 90, -span_angle_*180.0/3.1415);
    path_.lineTo(B1);
    path_.arcTo(rect_outer_, 90 - span_angle_*180.0/3.1415, span_angle_*180.0/3.1415);
}

bool Dipole::IsOn() const {
    return on_;
}

bool Dipole::SetState(bool on) {
    bool state_changed = (on != on_);
    on_ = on;
    return state_changed;
}

QRectF Dipole::boundingRect() const
{
    return QRectF();
}

void Dipole::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QPen pen;
    pen.setWidth(2);
    QColor brown_transparent(brown_);
    brown_transparent.setAlpha(128);
    pen.setBrush(brown_transparent);
    pen.setColor(brown_);
    painter->setPen(pen);
    painter->setBrush(brown_transparent);

    painter->drawPath(path_);
}

}

