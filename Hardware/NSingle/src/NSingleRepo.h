#ifndef MEDICYCCS2_REPO_H
#define MEDICYCCS2_REPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>
#include <QStandardPaths>

#include "NSingleDbusAddress.h"
#include "NSingleConfig.h"
#include "MultiplexConfig.h"
#include "NSingleSetPoint.h"
#include "StateReply.h"
#include "StateCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

// This should be moved somewhere more appropriately
struct BPLevel {
    double setpoint;
    int duration;
};

class NSingleRepo : public QObject {
  Q_OBJECT
public:
    NSingleRepo(QString db_suffix);
    ~NSingleRepo();
    // Get all available actual nsingles, thus not including virtual nsingles to multiplexed channels
    QStringList GetHardNSingles() const; // todo qstringlist to vector<QString>
    // Get all available nsingles from the provided list of subsystems, actual and virtual
    std::vector<QString> GetNSingles(QStringList sub_systems) const;
    // Short for getting all available nsingles for a single sub_system, actual and virtual
    std::vector<QString> GetNSingles(QString sub_system) const;
    // Get all available and unavailable nsingles for a single subsystem, actual and virtual
    QMap<QString, bool> GetAllNSingles(QString sub_system) const;
    QString GetNSingleShortName(QString nsingle) const;
    // Todo get multiplexed
    //QStringList GetMultiplexedNSingles() const;
    // Get the name and availability of the nsingles for a given subsystem and configuration
    QMap<QString, bool> GetNSingles(QString sub_system, QString cyclo_config);

    hw_nsingle::NSingleConfig GetNSingleConfig(QString name) const;
    QStringList GetMultiplexedChannels(QString nsingle) const;
    hw_nsingle::MultiplexConfig GetMultiplexConfig(QString nsingle) const;
    hw_nsingle::NSingleDbusAddress GetNSingleDbusAddress(QString nsingle) const;
    hw_nsingle::NSingleSetPoint GetNSingleSetPoint(QString cyclo_config, QString nsingle);
    void SaveNSingleSetPoint(QString cyclo_config, QString nsingle, hw_nsingle::NSingleSetPoint set_point);

    std::vector<BPLevel> GetBPLevels() const;

private:
    hw_nsingle::StateReplyConfig GetNSingleStateReplyConfig(QString name) const;
    hw_nsingle::StateCommandConfig GetNSingleStateCommandConfig(QString name) const;

    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
