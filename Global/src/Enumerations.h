#ifndef GLOBAL_ENUMS_H
#define GLOBAL_ENUMS_H

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>
#include <QStringList>
#include <QMetaEnum>

namespace medicyc::cyclotroncontrolsystem::global {

class Enumerations {
    Q_GADGET

public:
    enum SubSystem { BOBINEPRINCIPALE = 0,
                BOBINESDECORRECTION = 1,
                INJECTION = 2,
                EJECTION = 3,
                VDF = 4,
                SOURCE = 5,
                COOLINGSYSTEM = 6,
                VACUUM = 7,
                OTHER = 8};
    Q_ENUM(SubSystem)

    static QString SubSystemToString(SubSystem ss) {
        return QMetaEnum::fromType<Enumerations::SubSystem>().valueToKey(ss);
    }
};

} // namespace

#endif
