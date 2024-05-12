#include "BarcoMessage.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

BarcoMessage::BarcoMessage() {}

BarcoMessage::BarcoMessage(int id, SEVERITY severity, QString message, bool enabled)
    : id_(id), severity_(severity), message_(message), enabled_(enabled) {

}

bool operator==(const BarcoMessage& c1, const BarcoMessage& c2) {
    return (c1.id() == c2.id() &&
            c1.severity() == c2.severity() &&
            c1.message() == c2.message() &&
            c1.enabled() == c2.enabled());
}

bool operator!=(const BarcoMessage &c1, const BarcoMessage& c2) {
    return !(c1 == c2);
}

}

