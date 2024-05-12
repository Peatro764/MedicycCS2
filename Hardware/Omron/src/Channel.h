#ifndef OMRONCHANNEL_H
#define OMRONCHANNEL_H


#include <QObject>
#include <QDebug>
#include <QString>

#include "FINS.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class OMRON_EXPORT Channel {
public:
    Channel();
    Channel(QString name, QString system, uint8_t node, FINS_MEMORY_AREA area, uint16_t address, uint8_t bit, bool impuls);
    QString name() const { return name_; }
    QString system() const { return system_; }
    uint8_t node() const { return node_; }
    FINS_MEMORY_AREA fins_memeory_area() const { return fins_memory_area_; }
    uint16_t address() const { return address_; }
    uint8_t bit() const { return bit_; }
    bool impuls() const { return impuls_; }

private:
    QString name_;
    QString system_;
    uint8_t node_;
    FINS_MEMORY_AREA fins_memory_area_;
    uint16_t address_;
    uint8_t bit_;
    bool impuls_;
};

bool operator==(const Channel &c1, const Channel& c2);
bool operator!=(const Channel &c1, const Channel& c2);

}

#endif // OMRONCHANNEL
