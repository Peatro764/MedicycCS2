#include "Channel.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

Channel::Channel() {}

Channel::Channel(QString name, int card, int block, int address)
    : name_(name), card_(card), block_(block), address_(address) {}

bool operator==(const Channel &c1, const Channel& c2) {
    return (c1.Card() == c2.Card() &&
            c1.Address() == c2.Address() &&
            c1.Block() == c2.Block());
}

bool operator!=(const Channel &c1, const Channel& c2) {
    return !(c1 == c2);
}

}
