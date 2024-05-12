#include "RadiationMonitorRepo.h"

#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QSqlError>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

RadiationMonitorRepo::RadiationMonitorRepo(QString db_suffix)
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

    qDebug() << "NSingleRepo::Repo Host " << hostName
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

RadiationMonitorRepo::~RadiationMonitorRepo() {
    QSqlDatabase::removeDatabase(db_name_);
}

Configuration RadiationMonitorRepo::GetConfiguration(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "RadiationMonitorRepo::GetConfiguration Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT timestamp, name, ip, port, connection_timeout, read_timeout, write_timeout, mode_functionnement, preampli_type, preampli_gamme, instantaneous_lam, "
                  "integrated1_lam, integrated2_lam, command_confirmation, include_date_in_measurement, "
                  "include_raw_in_measurement, instantaneous_time, instantaneous_elements, integrated1_time, "
                  "integrated2_time, number_of_measurements, instantaneous_conversion_coefficient, integrated_conversion_coefficient, "
                  "integrated_threshold, threshold_A, threshold_B, threshold_C "
                  "FROM RadiationMonitorConfiguration WHERE name = :name");

    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << "RadiationMonitorRepo::GetConfiguration failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    if (static_cast<int>(query.size()) != 1) {
        qWarning() << "PTRepo::GetBaliseConfiguration Query returned wrong number of rows: " << query.size();
        throw std::runtime_error(((int)query.size() < 1) ? "There is no balise configuration" : "Multiple balise configurations were returned");
    }

    if (!query.next()) {
        throw std::runtime_error("Query::next failed");
    }
    return Configuration(query.value(0).toDateTime(), // timestamp
                         query.value(1).toString(), // name
                         query.value(2).toString(), // ip
                         query.value(3).toInt(), // port
                         query.value(4).toInt(), // connection_timeout
                         query.value(5).toInt(), // read_timeout
                         query.value(6).toInt(), // write_timeout
                         query.value(7).toInt(), // mode functionnement
                         query.value(8).toInt(), // preamplitype
                         query.value(9).toInt(), // preampligamme
                         query.value(10).toBool(), // instantaneous lam
                         query.value(11).toBool(), // integrated1 lam
                         query.value(12).toBool(), // integrated2 lam
                         query.value(13).toBool(), // command confirmation
                         query.value(14).toBool(), // include date in measurement
                         query.value(15).toBool(), // include raw in measurement
                         query.value(16).toInt(), // instantaneous time
                         query.value(17).toInt(), // instantaneous elements
                         query.value(18).toInt(), // integrated1 time
                         query.value(19).toInt(), // integrated2 time
                         query.value(20).toInt(), // number of measurements
                         query.value(21).toDouble(), // instantaneous conversion coefficient
                         query.value(22).toDouble(), // integrated conversion coefficient
                         query.value(23).toDouble(), // integrated threshold
                         query.value(24).toDouble(), // threshold A
                         query.value(25).toDouble(), // threshold B
                         query.value(26).toDouble()); // threshold C
}

std::vector<QString> RadiationMonitorRepo::GetRadiationMonitorNames() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "RadiationMonitorRepo::GetRadiationMonitorNames Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT name FROM RadiationMonitorConfiguration");

    if (!query.exec()) {
        qWarning() << "RadiationMonitorRepo::GetRadiationMonitors failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }

    std::vector<QString> names;
    while (query.next()) {
        names.push_back(query.value(0).toString());
    }
    return names;
}


}
