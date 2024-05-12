#include "GlobalRepo.h"

#include <QSettings>
#include <QSqlError>
#include <QDebug>

#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::global {

GlobalRepo::GlobalRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    qDebug() << "DB name " << db_name_;
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();
    int timeout = settings.value(db_name_ + "/timeout", 3000).toInt();

    qDebug() << "GlobalRepo::GlobalRepo Host " << hostName
             << " database " << databaseName
             << " port " << port
             << " username " << userName
             << " password " << password;

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", db_name_);
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
}

GlobalRepo::~GlobalRepo() {
    QSqlDatabase::removeDatabase(db_name_);
}

QMap<QDateTime, QStringList> GlobalRepo::GetAvailableConfigs() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "GlobalRepo::GetAvailableConfigs Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT timestamp, name, comments FROM CycloConfig ORDER BY timestamp ASC");

    if (!query.exec()){
        db.close();
        qWarning() << QString("GlobalRepo::GetAvailableConfigs Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }

    QMap<QDateTime, QStringList> configs;
    while(query.next()) {
        if(configs.contains(query.value(0).toDateTime())) {
            qWarning() << "Cyclo config date " + query.value(0).toDateTime().toString() + " is a duplicate, the previous will be overwritten";
        }
        QStringList name_comments = { query.value(1).toString(), query.value(2).toString() };
        configs[query.value(0).toDateTime()] = name_comments;
    }

    return configs;
}

void GlobalRepo::SetActiveConfig(QString subsystem, QString config) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "GlobalRepo::SetActiveConfig Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("INSERT INTO ActiveConfig(timestamp, subsystem, config) "
                  "VALUES(:timestamp, :subsystem, :config)");
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":subsystem", subsystem);
    query.bindValue(":config", config);

    if (!query.exec()){
        db.close();
        qWarning() << QString("GlobalRepo::SetActiveConfig Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }

}

QString GlobalRepo::GetActiveConfig(QString subsystem) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "GlobalRepo::GetActiveConfig Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT config FROM ActiveConfig "
                  "WHERE subsystem = :subsystem ORDER BY timestamp DESC LIMIT 1");
    query.bindValue(":subsystem", subsystem);

    if (!query.exec()){
        db.close();
        qWarning() << QString("GlobalRepo::GetActiveConfig Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }

    if (!query.next()) {
        qWarning() << QString("GlobalRepo::GetActiveConfig No Data");
        throw std::runtime_error("Pas de données");
    }

    QString config = query.value(0).toString();
    return config;
}


}
