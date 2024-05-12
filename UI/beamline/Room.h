#ifndef ROOM_H
#define ROOM_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include <QSizeF>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Room : public QGraphicsObject, public BeamItem {
    Q_OBJECT

public:
    Room(QString title, double size, BeamItem item);
    QRectF boundingRect() const;
    void SetBeam(bool active, bool beam_delivered);
    bool BeamDelivered() const { return beam_delivered_; }
    QString Name() const { return title_; }

signals:
    void SIGNAL_BeamDelivered();

private:
    void PlayBeamDelivered();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString title_;
    QGraphicsTextItem label_;
    QRectF bounding_box_outer_;
    QRectF bounding_box_inner_;
    QColor beam_on_color_ = Qt::red;
    QColor beam_hypothetical_color_ = QColor("cornflowerblue");
    QColor beam_off_color_ = Qt::black;
    bool active_ = false; // the room is the currently selected beam path
    bool beam_delivered_ = false; // beam is delivered to room
};

}

#endif
