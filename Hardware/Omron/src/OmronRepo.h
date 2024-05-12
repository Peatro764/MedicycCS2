#ifndef MEDICYCCS2_OMRONREPO_H
#define MEDICYCCS2_OMRONREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "Omron.h"
#include "BarcoMessage.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class OmronRepo : public QObject {
  Q_OBJECT
public:
    OmronRepo(QString db_suffix);
    ~OmronRepo();
    bool Connect(int timeout);
    void Disconnect();
    bool IsConnected();

    QStringList GetOmronSystemNames() const;
    QStringList GetOmronChannelNames() const;
    medicyc::cyclotroncontrolsystem::hardware::omron::Channel GetOmronChannel(QString name) const;
    QMap<QString, medicyc::cyclotroncontrolsystem::hardware::omron::Channel> GetChannels(QString system);
    QMap<int, BarcoMessage> GetBarcoMessages() const;

signals:

private:
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
};

}

#endif
