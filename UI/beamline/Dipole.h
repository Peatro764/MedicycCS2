#ifndef DIPOLE_H
#define DIPOLE_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QGraphicsTextItem>
#include <QSizeF>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Dipole : public QGraphicsObject, public BeamItem {
    Q_OBJECT

public:
    Dipole(QString name, double span_angle, BeamItem item);
    bool SetState(bool on); // returns true if state changed
    bool IsOn() const;
    bool IsOff() const { return !IsOn(); }
    QRectF boundingRect() const;

signals:

private:
    void Build();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString name_;
    double distance_;
    double span_angle_;
    bool on_ = false;
    QPainterPath path_;
    QRect rect_inner_;
    QRect rect_outer_;
    QPoint A1, A2, B1, B2;
    QPoint p1, p2, p0;
    QGraphicsTextItem label_;
    const QColor brown_ = QColor("#C4A484");
    const double HALF_LENGTH_ = 40;
};

}

#endif
