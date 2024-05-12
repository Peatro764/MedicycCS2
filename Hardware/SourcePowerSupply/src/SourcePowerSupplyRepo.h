#ifndef SOURCEPOWERSUPPLYREPO_H
#define SOURCEPOWERSUPPLYREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "PowerSupplyConfig.h"
#include "PowerSupplyDbusAddress.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

class SourcePowerSupplyRepo : public QObject {
  Q_OBJECT

public:
    SourcePowerSupplyRepo(QString name);
    ~SourcePowerSupplyRepo();
    bool Connect(int timeout);
    void Disconnect();
    bool IsConnected();

    QStringList GetPowerSupplies() const;
    PowerSupplyDbusAddress GetDbusAddress(QString power_supply) const;
    PowerSupplyConfig GetConfig(QString name) const;
    double GetSetPoint(QString name, QString unit, QString config) const;

private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
