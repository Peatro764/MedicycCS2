#include "ADCUtil.h"

#include <bitset>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

namespace util {
    QByteArray CheckSum(QByteArray data) {
        if (data.isEmpty() || data.isNull()) {
            return QByteArray("00");
        } else {
            int crc(data.at(0));
            for(int i = 1; i < data.length(); i++) {
                crc ^= data.at(i);
            }
            return QString("%1").arg(crc, 2, 16, QLatin1Char( '0' )).toUpper().toLatin1();
        }
    }

    int Hash(int card, int address) {
        return (card * 16 + address);
    }

}

}
