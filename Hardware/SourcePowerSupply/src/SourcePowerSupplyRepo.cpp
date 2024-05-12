#include "SourcePowerSupplyRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>
#include <QStandardPaths>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

SourcePowerSupplyRepo::SourcePowerSupplyRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "SourcePowerSupplyRepo::SourcePowerSupplyRepo Host " << hostName
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

SourcePowerSupplyRepo::~SourcePowerSupplyRepo() {
    Disconnect();
}

bool SourcePowerSupplyRepo::Connect(int timeout) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
    if (db.open()) {
        qDebug() << "SourcePowerSupplyRepo::Connect Connection to repo established";
        return true;
    } else {
        qCritical() << "SourcePowerSupplyRepo::Connect Failed: " + db.lastError().text();
        return false;
    }
}

void SourcePowerSupplyRepo::Disconnect() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.close();
}

bool SourcePowerSupplyRepo::IsConnected() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.isOpen();
}

QStringList SourcePowerSupplyRepo::GetPowerSupplies() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM SourcePowerSupply ORDER BY name ASC");

    if (!query.exec()) {
        qWarning() << "SourcePowerSupplyRepo::GetPowerSupplies Query failed: " << query.lastError();
        throw std::runtime_error("Db query for power supplies failed");
    }

    QStringList power_supplies;
    while (query.next()) {
        power_supplies.push_back(query.value(0).toString());
    }
    return power_supplies;
}

PowerSupplyDbusAddress SourcePowerSupplyRepo::GetDbusAddress(QString power_supply) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "SourcePowerSupplyRepo::GetDbusAddress Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT interface_address, object_name FROM SourcePowerSupplyDBusAddress WHERE name = :name");
    query.bindValue(":name", power_supply);

    if (!query.exec()) {
        qWarning() << "SourcePowerSupplyRepo::GetDbusAddress Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for power supply dbus address failed");
    }

    if (!query.next()) {
        qWarning() << QString("PowerSupplyRepo::GetDbusAddress No dbus address found for power supply : ") << power_supply;
        throw std::runtime_error("No dbus address found for power supply");
    }

    return PowerSupplyDbusAddress(power_supply,
                                  query.value(0).toString(),
                                  query.value(1).toString());
}


PowerSupplyConfig SourcePowerSupplyRepo::GetConfig(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name, regulated_param, heat_up_time, "
                  "reg_tolerance, reg_near_zero, reg_near_target, reg_lower_limit, reg_upper_limit, reg_buffer_size, "
                  "act_tolerance, act_near_zero, act_near_target, act_lower_limit, act_upper_limit, act_buffer_size, "
                  "voltage_unit, current_unit FROM "
                  "SourcePowerSupply where name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "SourcePowerSupplyRepo::GetConfig Query failed: " << query.lastError();
        throw std::runtime_error("Db query for config failed");
    }

    if (!query.next()) {
        qWarning() << "SourcePowerSupplyRepo::GetConfig Empty set";
        throw std::runtime_error("Db query for config failed");
    }

    QString regulated_param = query.value(1).toString();
    int heat_up_time = query.value(2).toInt();
    RegulatedParameterConfig reg_config(query.value(3).toDouble(), // tolerance
                                        query.value(4).toDouble(), // near_zero
                                        query.value(5).toDouble(), // near_target
                                        query.value(6).toDouble(), // lower_limit
                                        query.value(7).toDouble(), // upper_limit
                                        query.value(8).toInt()); // buffer_size
    RegulatedParameterConfig act_config(query.value(9).toDouble(), // tolerance
                                        query.value(10).toDouble(), // near_zero
                                        query.value(11).toDouble(), // near_target
                                        query.value(12).toDouble(), // lower_limit
                                        query.value(13).toDouble(), // upper_limit
                                        query.value(14).toInt()); // buffer_size
    QString voltage_unit = query.value(15).toString();
    QString current_unit = query.value(16).toString();
    return PowerSupplyConfig(name, regulated_param, heat_up_time, reg_config, act_config, voltage_unit, current_unit);
}

double SourcePowerSupplyRepo::GetSetPoint(QString name, QString unit, QString config) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT value FROM SourcePowerSupplySetPoint "
                  "WHERE power_supply_name = :name AND cyclo_config = :config AND unit = :unit");
    query.bindValue(":name", name);
    query.bindValue(":config", config);
    query.bindValue(":unit", unit);

    if (!query.exec()) {
        qWarning() << "SourcePowerSupplyRepo::GetSetPoint Query failed: " << query.lastError();
        throw std::runtime_error("Db query for setpoint failed");
    }

    if (!query.next()) {
        qWarning() << "SourcePowerSupplyRepo::GetSetPoint Empty set";
        throw std::runtime_error("Db query for setpoint failed");
    }

    double set_point = query.value(0).toDouble();
    return set_point;
}


}
