#include "IORRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>
#include <QStandardPaths>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

IORRepo::IORRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "IORRepo::IORReop Host " << hostName
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
}

IORRepo::~IORRepo() {
    Disconnect();
}

bool IORRepo::Connect(int timeout) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
    if (db.open()) {
        qDebug() << "IORRepo::Connect Connection to repo established";
        return true;
    } else {
        qCritical() << "IORRepo::Connect Failed: " + db.lastError().text();
        return false;
    }
}

void IORRepo::Disconnect() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.close();
}

bool IORRepo::IsConnected() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.isOpen();
}

QStringList IORRepo::GetIORChannelNames() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM IORChannel");

    if (!query.exec()) {
        qWarning() << "IORRepo::GetIORChannelNames Query failed: " << query.lastError();
        throw std::runtime_error("Db query for IOR channel names failed");
    }

    QStringList channel_names;
    while (query.next()) {
        channel_names.push_back(query.value(0).toString());
    }
    return channel_names;
}

QStringList IORRepo::GetIORChannelNames(QString system) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM IORChannel WHERE system = :system and enabled = true");
    query.bindValue(":system", system);

    if (!query.exec()) {
        qWarning() << "IORRepo::GetIORChannelNames Query failed: " << query.lastError();
        throw std::runtime_error("Db query for IOR channel names failed");
    }

    QStringList channel_names;
    while (query.next()) {
        channel_names.push_back(query.value(0).toString());
    }
    return channel_names;
}


ior::Channel IORRepo::GetIORChannel(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name, card, block, address "
                  "FROM IORChannel WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("IORRepo::GetIORChannel Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for IOR channel failed");
    }

    if (query.size() > 1) {
        qWarning() << "IORRepo::GetIORChannel Multiple IORs was found having name: " << name;
        throw std::runtime_error("Multiple IORs with the same name was found");
    }

    if (!query.next()) {
        qWarning() << QString("IORRepo::GetIORChannel No ADC found with name : ") << name;
        throw std::runtime_error("No IOR with the given name was found");
    }

    return ior::Channel(query.value(0).toString(),
                        query.value(1).toInt(),
                        query.value(2).toInt(),
                        query.value(3).toInt());
}

}
