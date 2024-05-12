#include "BeamPoint.h"

#include <algorithm>
#include <math.h>
#include <stdexcept>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

BeamPoint::BeamPoint(QPoint p, BeamPoint *next)
    : p_(p), next_(next) {
}

QPoint BeamPoint::Coordinate() const {
    return p_;
}

QPoint BeamPoint::InterpolatedCoordinate(double distance) const {
    if (next_ != nullptr) {
        if (distance <= this->LegLength()) {
            return (distance / this->LegLength()) * (next_->Coordinate() - this->Coordinate());
        } else {
            return (next_->Coordinate() - this->Coordinate()) + next_->InterpolatedCoordinate(distance - this->LegLength());
        }
    } else {
        throw std::runtime_error("BeamPoint::InterpolatedCoordinate next is nullptr");
    }
}

double BeamPoint::PathLength() const {
    if (next_ != nullptr) {
        return (this->LegLength() + next_->PathLength());
    } else {
        return this->LegLength();
    }
}

int BeamPoint::NSegments() const {
    if (next_ != nullptr) {
        return 1 + next_->NSegments();
    } else {
        return 1;
    }
}

double BeamPoint::LegAngle(double distance) const {
    if (next_ == nullptr) {
        throw std::runtime_error("BeamPoint::LegAngle next is nullptr");
    }
    double dY = (this->Coordinate().y() - next_->Coordinate().y());
    double dX = (next_->Coordinate().x() - this->Coordinate().x());
    if (distance <= this->LegLength()) {
        return std::atan(dY / dX);
    } else {
        return next_->LegAngle(distance - this->LegLength());
    }
}

double BeamPoint::LegLength() const {
    if (next_ != nullptr) {
        QPoint p_next = next_->Coordinate();
        return std::sqrt(std::pow(p_next.x() - p_.x(), 2) + std::pow(p_next.y() - p_.y(), 2));
    } else {
        return 0.0;
    }
}

void BeamPoint::AddAngles(std::vector<double>* angles) {
    if (next_ != nullptr) {
        angles->push_back(this->LegAngle(0));
        next_->AddAngles(angles);
    } else {
        return;
    }
}

void BeamPoint::AddLengths(std::vector<double>* lengths) {
    if (next_ != nullptr) {
        lengths->push_back(this->LegLength());
        next_->AddLengths(lengths);
    } else {
        return;
    }
}

void BeamPoint::AddToPath(QPolygon* path, double distance) const {
    double distance_on_current_leg = std::min(distance, this->LegLength());
    *path << (this->Coordinate() + this->InterpolatedCoordinate(distance_on_current_leg));
    double remaining_distance = (distance - distance_on_current_leg);
    if (remaining_distance > 0.00001) {
        if (next_ != nullptr) {
            next_->AddToPath(path, remaining_distance);
        } else {
            throw std::runtime_error("BeamPoint::AddToPath remaining distance non-null but next item is nullptr");
        }
    }
}

void BeamPoint::Print() {
    qDebug() << this->Coordinate();
    if (next_ != nullptr) {
        next_->Print();
    } else {
        qDebug() << "End";
    }
}

}
