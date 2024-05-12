#include "Channel.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

Channel::Channel() {}

Channel::Channel(QString name, QString system, uint8_t node, FINS_MEMORY_AREA area, uint16_t address, uint8_t bit, bool impuls)
    : name_(name), system_(system), node_(node), fins_memory_area_(area), address_(address), bit_(bit), impuls_(impuls) {}

bool operator==(const Channel& c1, const Channel& c2) {
    return (c1.name() == c2.name() &&
            c1.system() == c2.system() &&
            c1.node() == c2.node() &&
            c1.fins_memeory_area() == c2.fins_memeory_area() &&
            c1.address() == c2.address() &&
            c1.bit() == c2.bit() &&
            c1.impuls() == c2.impuls());
}

bool operator!=(const Channel &c1, const Channel& c2) {
    return !(c1 == c2);
}

}

