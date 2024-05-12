#ifndef BEAMLINE_BEAMITEM_H
#define BEAMLINE_BEAMITEM_H

#include <vector>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class BeamItem {
public:
    BeamItem();
    BeamItem(double distance, double angle, std::vector<Constants::BEAMLINE_PATHS> on_paths);
    double Distance() const;
    double Angle() const;
    bool OnSegment(Constants::BEAMLINE_PATHS path) const;
    Constants::BEAMLINE_PATHS BaseSegment() const;
    void PlayInsertClip(bool insert);
private:
    double distance_;
    double angle_;
    std::vector<Constants::BEAMLINE_PATHS> on_paths_;
};

}

#endif
