#ifndef MULTIPLEX_CONFIG_H
#define MULTIPLEX_CONFIG_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT MultiplexConfig
{
    Q_GADGET

public:
    MultiplexConfig();
    MultiplexConfig(QMap<QString, int> channels);
    void AddChannel(QString name, int address);
    std::bitset<8> SelectChannel(QString channel) const;
    QString Name(int address) const;
    bool ChannelExist(QString channel) const;
    std::vector<QString> channels() const;
    QString Iterate();

private:
    QMap<QString, int> channels_;
    QMap<QString, int>::iterator it_;
};

bool operator==(const MultiplexConfig &c1, const MultiplexConfig& c2);
bool operator!=(const MultiplexConfig &c1, const MultiplexConfig& c2);

} // namespace

#endif
