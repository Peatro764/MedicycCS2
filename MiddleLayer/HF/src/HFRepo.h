#ifndef MEDICYCCS2_MIDDLELAYER_HFREPO_H
#define MEDICYCCS2_MIDDLELAYER_HFREPO_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>

#include "Configuration.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

class HFRepo : public QObject {
  Q_OBJECT

public:
    HFRepo(QString db_suffix);
    ~HFRepo();
    bool Connect(int timeout);
    void Disconnect();
    bool IsConnected();
    Configuration GetConfiguration(int id) const;
    Configuration GetLastConfiguration(HFX hf) const;
    void SaveConfiguration(Configuration conf);

signals:
    void SIGNAL_NewConfigurationAdded(int id);

private slots:
    void Notification(const QString& channel, QSqlDriver::NotificationSource source, const QVariant &message);
    void CheckConnection();

private:
    bool SubscribeToConfigUpdateNotification();
    bool UnsubscribeFromConfigUpdateNotification();
    const QString DB_BASE_NAME_ = "hardwaredatabase";
    QString db_name_;
    const QString CONFIG_UPDATE_NOTIFICATION = "hfconfiguration_notification";
};

}

#endif
