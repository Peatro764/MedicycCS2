#ifndef CF_H
#define CF_H

#include "qpaintengine.h"
#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QGraphicsTextItem>
#include <QSizeF>

#include "BeamItem.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class CF : public QGraphicsObject, public BeamItem {
    Q_OBJECT
    Q_PROPERTY(QPointF pos_cf READ GetPosCF WRITE SetPosCF NOTIFY SIGNAL_PosCFChanged)

public:
    CF(QString title, BeamItem item);
    bool Inserted() const { return inserted_; }
    bool SetState(bool insert); // returns if state changed
    QRectF boundingRect() const;


signals:
    void SIGNAL_PosCFChanged(QPointF);

private:
    void PlayStateChange(bool inserted);
    QPointF GetPosCF() const { return pos_cf_; }
    void SetPosCF(QPointF& pos) { pos_cf_ = pos; update(); emit SIGNAL_PosCFChanged(pos); }
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPropertyAnimation *anim_;
    QString title_;
    bool inserted_ = false;
    QPointF pos_cf_;
    QPointF extract_pos_;
    QPointF insert_pos_;
    QPointF pos_boundary_;
    QSizeF size_cf_;
    QSizeF size_boundary_;
    QGraphicsTextItem label_;
};

}

#endif // CFITEM_H
