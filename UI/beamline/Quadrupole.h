#ifndef QUADRUPOLE_H
#define QUADRUPOLE_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include <QSizeF>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Quadrupole : public QGraphicsObject, public BeamItem {

public:
    Quadrupole(QString title, BeamItem item);
    QRectF boundingRect() const;

signals:

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString title_;
    QRectF rect_;
    QGraphicsTextItem label_;
    const QColor brown_ = QColor("#C4A484");
};

}

#endif
