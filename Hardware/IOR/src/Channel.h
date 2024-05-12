#ifndef IOR_CHANNEL_H
#define IOR_CHANNEL_H

#include "IOR_global.h"
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class IOR_EXPORT Channel {
public:
    Channel();
    Channel(QString name, int card, int block, int address);
    QString Name() const { return name_; }
    int Card() const { return card_; }
    int Block() const { return block_; }
    int Address() const { return address_; }

private:
    QString name_;
    int card_;
    int block_;
    int address_;
};

bool operator==(const Channel &c1, const Channel& c2);
bool operator!=(const Channel &c1, const Channel& c2);

}

#endif
