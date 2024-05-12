#include "Channel.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

Channel::Channel(){}

Channel::Channel(QString name, int card, int address, double conversion_factor, QString unit, double sim_value, double sim_step)
    : name_(name), card_(card), address_(address), conversion_factor_(conversion_factor), unit_(unit), sim_value_(sim_value), sim_step_(sim_step) {}

bool operator==(const Channel &c1, const Channel& c2) {
    return (c1.Name() == c2.Name() &&
            c1.Card() == c2.Card() &&
            c1.Address() == c2.Address() &&
            std::abs(c1.ConversionFactor() - c2.ConversionFactor()) < 0.00001 &&
            c1.Unit() == c2.Unit());
}

bool operator!=(const Channel &c1, const Channel& c2) {
    return !(c1 == c2);
}

}
