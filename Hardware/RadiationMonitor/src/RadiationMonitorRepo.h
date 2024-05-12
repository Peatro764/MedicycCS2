#ifndef MEDICYCCS2_RADIATIONMONITORREPO_H
#define MEDICYCCS2_RADIATIONMONITORREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>

#include "Configuration.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class RadiationMonitorRepo : public QObject {
    Q_OBJECT
public:
    RadiationMonitorRepo(QString db_suffix);
    ~RadiationMonitorRepo();
    Configuration GetConfiguration(QString name) const;
    std::vector<QString> GetRadiationMonitorNames() const;
private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;

};

}

#endif // RADIATIONMONITORREPO_H
