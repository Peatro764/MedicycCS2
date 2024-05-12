#ifndef BEAMPATH_H
#define BEAMPATH_H

#include <QString>
#include <QPoint>
#include <QPolygon>

#include "BeamPoint.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class BeamPath {

public:
    BeamPath(BeamPoint* head);
    QPoint Coordinate(double distance) const;
    double TotalLength() const;
    int NSegments() const;
    std::vector<double> SegmentLengths() const;
    std::vector<double> SegmentAngles() const;
    double SegmentAngle(double distance) const;
    QPolygon Path(double distance) const;
    void Print();

private:
    BeamPoint* head_;
};

}

#endif
