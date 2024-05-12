#ifndef IOR_UTIL_H
#define IOR_UTIL_H

#include <QObject>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

namespace ior_util {
    QString CheckSum(QString data);
    int Hash(int card, int block, int address);
}

const int NCARDS = 4;
const int NCOLSPERBIT = 9;
const int NBLOCKSPERCARD = 3;
const int NBITSPERBLOCK = 8;

}

#endif
