#ifndef DT_H
#define DT_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QGraphicsTextItem>
#include <QSizeF>
#include <QSound>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class DT : public QGraphicsObject, public BeamItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos_dt READ GetPosDT WRITE SetPosDT NOTIFY SIGNAL_PosDTChanged)

public:
    DT(BeamItem item);
    bool Inserted() const { return inserted_; }
    bool SetState(bool insert);
    QRectF boundingRect() const;

signals:
    void SIGNAL_PosDTChanged(QPointF);

private:
    void PlayStateChange(bool inserted);
    QPointF GetPosDT() const { return pos_dt_; }
    void SetPosDT(QPointF& pos) { pos_dt_ = pos; update(); emit SIGNAL_PosDTChanged(pos); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPropertyAnimation *anim_;
    QString title_;
    bool inserted_ = false;
    QPointF pos_dt_;
    QPointF extract_pos_;
    QPointF insert_pos_;
    QPointF pos_boundary_;
    QSizeF size_dt_;
    QSizeF size_boundary_;
    QGraphicsTextItem label_;
};

}

#endif
