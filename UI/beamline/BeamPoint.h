#ifndef BEAMPOINT_H
#define BEAMPOINT_H

#include <QPoint>
#include <QPolygon>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class BeamPoint {

public:
    BeamPoint(QPoint p, BeamPoint *next);
    BeamPoint(BeamPoint& point);
    QPoint Coordinate() const;
    QPoint InterpolatedCoordinate(double distance) const;
    double PathLength() const;
    int NSegments() const;
    double LegAngle(double distance) const;
    void AddAngles(std::vector<double>* angles);
    void AddLengths(std::vector<double>* lengths);
    void AddToPath(QPolygon* path, double distance) const;
    void Print();

private:
    double LegLength() const;
    QPoint p_;
    BeamPoint *next_;
};

}

#endif
