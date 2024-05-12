#include "NSingleUtil.h"

#include <bitset>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {
namespace util {

QString CheckSum(QString data) {
    if (data.isEmpty() || data.isNull()) {
        return QString("00");
    } else {
        int crc(data.at(0).unicode());
        for(int i = 1; i < data.length(); i++) {
            crc ^= data.at(i).unicode();
        }
        return QString("%1").arg(crc, 2, 16, QLatin1Char( '0' )).toUpper();
    }
}
}
} // ns
