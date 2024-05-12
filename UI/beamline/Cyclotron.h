#ifndef CYCLOTRON_H
#define CYCLOTRON_H

//#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QGraphicsTextItem>
#include <QSizeF>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Cyclotron : public QGraphicsObject {

public:
    Cyclotron(QString title, double size, double beam_pipe_angle, double beam_pipe_delta_angle);
    QRectF boundingRect() const;
    void SetBeam(bool on);

private:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    double beam_pipe_angle_;
    double beam_pipe_delta_angle_;
    QGraphicsTextItem label_;
    QRectF bounding_box_outer_;
    QRectF bounding_box_inner_;
    QColor beam_on_color_ = Qt::red;
    QColor beam_off_color_ = QColor("cornflowerblue");
    bool beam_on_ = false;
    QColor color_inner_;
};

}

#endif
