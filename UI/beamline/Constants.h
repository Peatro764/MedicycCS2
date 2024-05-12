#ifndef BEAMLINE_CONSTANTS_H
#define BEAMLINE_CONSTANTS_H

#include <QString>
#include <QColor>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

namespace Constants {
const double PIPE_OUTER_RADIUS = 16;
const double PIPE_INNER_RADIUS = 10;
const int BEAM_ITEM_FONT_SIZE = 15;
const int BEAM_ITEM_WIDTH = 12;
const int BEAM_ITEM_HEIGHT = 2*PIPE_INNER_RADIUS;
const double SIZE_MULTIPLIER = 0.27;
const double ROOM_DIAMETER = 70;
const double CYCLOTRON_DIAMETER = 150;
enum class BEAMLINE_PATHS { STRAIGHT, RD_M2, RD_M4, CLINICAL };
};

} // namespace

#endif

