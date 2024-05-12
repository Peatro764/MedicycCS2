#include "BeamItem.h"

#include <algorithm>
#include <stdexcept>
#include <QDebug>
#include <QSound>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

BeamItem::BeamItem() {}

BeamItem::BeamItem(double distance, double angle, std::vector<Constants::BEAMLINE_PATHS> on_paths)
    : distance_(distance), angle_(angle), on_paths_(on_paths) {
}

double BeamItem::Distance() const { return distance_; }

double BeamItem::Angle() const { return angle_; }

bool BeamItem::OnSegment(Constants::BEAMLINE_PATHS path) const {
    return (std::find(on_paths_.begin(), on_paths_.end(), path) != on_paths_.end());
}

Constants::BEAMLINE_PATHS BeamItem::BaseSegment() const {
    if (on_paths_.empty()) {
        throw std::runtime_error("No beamline segments");
    }
    Constants::BEAMLINE_PATHS front = on_paths_.front();
    return front;
};

void BeamItem::PlayInsertClip(bool insert) {
    if (insert) {
        QSound::play(":/sounds/item_inserted.wav");
    } else {
        QSound::play(":/sounds/item_extracted.wav");
    }
}


}
