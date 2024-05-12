#include "BeamPath.h"

#include <algorithm>
#include <math.h>
#include <stdexcept>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

BeamPath::BeamPath(BeamPoint* head)
    : head_(head) {
}

QPoint BeamPath::Coordinate(double distance) const {
    return head_->InterpolatedCoordinate(distance);
}

double BeamPath::TotalLength() const {
    return head_->PathLength();
}

int BeamPath::NSegments() const {
    return (head_->NSegments() - 1);
}

std::vector<double> BeamPath::SegmentAngles() const {
    std::vector<double> angles;
    head_->AddAngles(&angles);
    return angles;
}

std::vector<double> BeamPath::SegmentLengths() const {
    std::vector<double> lengths;
    head_->AddLengths(&lengths);
    return lengths;
}

QPolygon BeamPath::Path(double distance) const {
    if (distance > head_->PathLength()) {
        throw std::runtime_error("BeamPath::Path distance longer than total length");
    }
    QPolygon polygon;
    polygon << head_->Coordinate();
    head_->AddToPath(&polygon, distance);
    return polygon;
}

double BeamPath::SegmentAngle(double distance) const {
    return head_->LegAngle(distance);
}


void BeamPath::Print() {
    qDebug() << "Printing beam path";
    head_->Print();
}

}
