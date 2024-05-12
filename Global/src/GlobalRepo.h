#ifndef MEDICYCCS2_GLOBALREPO_H
#define MEDICYCCS2_GLOBALREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <map>
#include <QStandardPaths>

#include "Enumerations.h"

namespace medicyc::cyclotroncontrolsystem::global {

class GlobalRepo : public QObject {
  Q_OBJECT

public:
    GlobalRepo(QString db_suffix);
    ~GlobalRepo();

    QMap<QDateTime, QStringList> GetAvailableConfigs() const;
    void SetActiveConfig(QString subsystem, QString config);
    QString GetActiveConfig(QString subsystem);

private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
