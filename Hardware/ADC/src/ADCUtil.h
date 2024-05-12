#ifndef ADC_UTIL_H
#define ADC_UTIL_H

#include "ADC_global.h"

#include <QObject>
#include <QByteArray>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

namespace util {
    QByteArray CheckSum(QByteArray data);
    int Hash(int card, int address);
}

}

#endif
