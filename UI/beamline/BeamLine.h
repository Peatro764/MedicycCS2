#ifndef BEAMLINE_H
#define BEAMLINE_H

#include <QGraphicsObject>
#include <QPropertyAnimation>
#include <QSizeF>
#include <QPainterPath>
#include <cmath>

#include "Constants.h"
#include "BeamPath.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class BeamLine : public QGraphicsObject {
    Q_OBJECT

public:
    BeamLine();
    void SetBeam(Constants::BEAMLINE_PATHS segment, double distance, bool beam_on);
    BeamPath* GetBeamPath(Constants::BEAMLINE_PATHS path);

signals:

private:
    void BuildHypotheticalBeam(Constants::BEAMLINE_PATHS path);
    void BuildBeam(Constants::BEAMLINE_PATHS path, double distance);
    void BuildBeamPaths();
    void BuildBeamPipe();
    void BuildBeamErasers();
    QPolygon GetOuter1Pipe() const;
    QPolygon GetOuter2Pipe() const;
    QPolygon GetInner1Pipe() const;
    QPolygon GetInner2Pipe() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QMap<Constants::BEAMLINE_PATHS, BeamPath*> beam_paths_;

    QMap<Constants::BEAMLINE_PATHS, QPainterPath> beam_erasers_;
    QPainterPath beam_hypothetical_;
    QPainterPath beam_actual_;

    QPainterPath pipe_inner1_;
    QPainterPath pipe_inner2_;
    QPainterPath pipe_outer1_;
    QPainterPath pipe_outer2_;
};

}

#endif // CFITEM_H
