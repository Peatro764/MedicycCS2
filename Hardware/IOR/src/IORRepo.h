#ifndef MEDICYCCS2_IORREPO_H
#define MEDICYCCS2_IORREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "IOR.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class IOR_EXPORT IORRepo : public QObject {
  Q_OBJECT

public:
    IORRepo(QString name);
    ~IORRepo();
    bool Connect(int timeout);
    void Disconnect();
    bool IsConnected();

    QStringList GetIORChannelNames() const;
    QStringList GetIORChannelNames(QString system) const;
    ior::Channel GetIORChannel(QString name) const;

private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
