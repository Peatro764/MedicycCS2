#ifndef QZ_H
#define QZ_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QGraphicsTextItem>
#include <QSizeF>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class QZ : public QGraphicsObject, public BeamItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos_qz READ GetPosQZ WRITE SetPosQZ NOTIFY SIGNAL_PosQZChanged)

public:
    QZ(QString title, BeamItem item);
    bool Inserted() const { return inserted_; }
    bool SetState(bool insert); // returns true if state changed
    QRectF boundingRect() const;

signals:
    void SIGNAL_PosQZChanged(QPointF);

private:
    void PlayStateChange(bool inserted);
    QPointF GetPosQZ() const { return pos_qz_; }
    void SetPosQZ(QPointF& pos) { pos_qz_ = pos; update(); emit SIGNAL_PosQZChanged(pos); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPropertyAnimation *anim_;
    QString title_;
    bool inserted_ = false;
    QPointF pos_qz_;
    QPointF extract_pos_;
    QPointF insert_pos_;
    QPointF pos_boundary_;
    QSizeF size_qz_;
    QSizeF size_boundary_;
    QGraphicsTextItem label_;
};

}

#endif
