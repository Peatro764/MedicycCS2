#include "HFRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>
#include <QStandardPaths>
#include <QTimer>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

HFRepo::HFRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "HFRepo::HFRepo Host " << hostName
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
    assert(QObject::connect(db.driver(), SIGNAL(notification(const QString&, QSqlDriver::NotificationSource, const QVariant&)), this, SLOT(Notification(const QString&, QSqlDriver::NotificationSource, const QVariant&))));

    if(Connect(5)) {
        qDebug() << "Connection successful";
    } else {
        qWarning() << "Connection failed";
    }

    if (SubscribeToConfigUpdateNotification()) {
        qDebug() << "Subscription to psql notification successful";
    } else {
        qWarning() << "Subscription to psql notification failed";
    }

    QTimer *tConnection = new QTimer(this);
    tConnection->setInterval(60000);
    tConnection->setSingleShot(false);
    QObject::connect(tConnection, &QTimer::timeout, this, &HFRepo::CheckConnection);
    tConnection->start();
}

HFRepo::~HFRepo() {
    Disconnect();
}

void HFRepo::CheckConnection() {
    auto db = QSqlDatabase::database(db_name_);
    if (!db.isOpen()) {
        qWarning() << "HFRepo::CheckConnection Database connection is broken, reconnecting";
        if (db.open()) {
            qDebug() << "HFRepo::CheckConnection Reconnection successful";
        } else {
            qWarning() << "HFRepo::CheckConnection Reconnection unsuccessful";
        }
    }
}

Configuration HFRepo::GetConfiguration(int id) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT hf, timestamp,"
                  "accord_voltage_lower, accord_voltage_upper, accord_phase_lower, accord_phase_upper,"
                  "membrane_min, membrane_max, membrane_clearance,"
                  "niveau_preon_value, niveau_preoff_value, niveau_startup_final_value, niveau_shutdown_initial_value, niveau_max,"
                  "voltage_max, voltage_delta_down, voltage_delta_up, voltage_post_accord "
                  "FROM HFConfiguration "
                  "WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "HFRepo::GetConfiguration Query failed: " << query.lastError();
        throw std::runtime_error("Db query for HF configuration failed");
    }

    if (query.size() > 1) {
        qWarning() << "HFRepo::GetConfiguration Multiple configurations were found having id " << id;
        throw std::runtime_error("Multiple HF configurations were found having the same id");
    }

    if (!query.next()) {
        qWarning() << QString("HFRepo::GetConfiguration No configuration found with id : ") << id;
        throw std::runtime_error("No HF configuration was found with the given id");
    }

    return Configuration(static_cast<HFX>(query.value(0).toInt()),
                         query.value(1).toDateTime(),
                         query.value(2).toDouble(),
                         query.value(3).toDouble(),
                         query.value(4).toDouble(),
                         query.value(5).toDouble(),
                         query.value(6).toDouble(),
                         query.value(7).toDouble(),
                         query.value(8).toDouble(),
                         query.value(9).toDouble(),
                         query.value(10).toDouble(),
                         query.value(11).toDouble(),
                         query.value(12).toDouble(),
                         query.value(13).toDouble(),
                         query.value(14).toDouble(),
                         query.value(15).toDouble(),
                         query.value(16).toDouble(),
                         query.value(17).toDouble());
}

Configuration HFRepo::GetLastConfiguration(HFX hf) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT id from HFConfiguration "
                  "WHERE hf = :hf "
                  "ORDER BY timestamp desc LIMIT 1");
    query.bindValue(":hf", static_cast<int>(hf));

    if (!query.exec()) {
        qWarning() << "HFRepo::GetLastConfiguration Query failed: " << query.lastError();
        throw std::runtime_error("Db query for HF last configuration failed");
    }

    if (!query.next()) {
        qWarning() << QString("HFRepo::GetLastConfiguration No configuration found with for HF : ") << static_cast<int>(hf);
        throw std::runtime_error("No HF configuration was found");
    }

    return this->GetConfiguration(query.value(0).toInt());
}

void HFRepo::SaveConfiguration(Configuration conf) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("INSERT INTO HFConfiguration(hf, timestamp, accord_voltage_lower, accord_voltage_upper,"
                  "accord_phase_lower, accord_phase_upper, membrane_min, membrane_max, membrane_clearance,"
                  "niveau_preon_value, niveau_preoff_value, niveau_startup_final_value, niveau_shutdown_initial_value, niveau_max,"
                  "voltage_max, voltage_delta_down, voltage_delta_up, voltage_post_accord) "
                  "VALUES(:hf, :timestamp, :accord_voltage_lower, :accord_voltage_upper,"
                  ":accord_phase_lower, :accord_phase_upper, :membrane_min, :membrane_max, :membrane_clearance,"
                  ":niveau_preon_value, :niveau_preoff_value, :niveau_startup_final_value, :niveau_shutdown_initial_value, :niveau_max,"
                  ":voltage_max, :voltage_delta_down, :voltage_delta_up, :voltage_post_accord)");
    query.bindValue(":hf", static_cast<int>(conf.hf()));
    query.bindValue(":timestamp", QDateTime::currentDateTime());
    query.bindValue(":accord_voltage_lower", conf.accord_voltage_lower());
    query.bindValue(":accord_voltage_upper", conf.accord_voltage_upper());
    query.bindValue(":accord_phase_lower", conf.accord_phase_lower());
    query.bindValue(":accord_phase_upper", conf.accord_phase_upper());
    query.bindValue(":membrane_min", conf.membrane_min());
    query.bindValue(":membrane_max", conf.membrane_max());
    query.bindValue(":membrane_clearance", conf.membrane_clearance());
    query.bindValue(":niveau_preon_value", conf.niveau_preon_value());
    query.bindValue(":niveau_preoff_value", conf.niveau_preoff_value());
    query.bindValue(":niveau_startup_final_value", conf.niveau_startup_final_value());
    query.bindValue(":niveau_shutdown_initial_value", conf.niveau_shutdown_initial_value());
    query.bindValue(":niveau_max", conf.niveau_max());
    query.bindValue(":voltage_max", conf.voltage_max());
    query.bindValue(":voltage_delta_down", conf.voltage_delta_down());
    query.bindValue(":voltage_delta_up", conf.voltage_delta_up());
    query.bindValue(":voltage_post_accord", conf.voltage_post_accord());

    if (!query.exec()) {
        qWarning() << "HFRepo::SaveConfiguration Query to insert Configuration failed: " << query.lastError();
        throw std::runtime_error("Db query failed");
    }
}


bool HFRepo::Connect(int timeout) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
    if (db.open()) {
        qDebug() << "HFRepo::Connect Connection to repo established";
        return true;
    } else {
        qCritical() << "HFRepo::Connect Failed: " + db.lastError().text();
        return false;
    }
}

void HFRepo::Disconnect() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.close();
}

bool HFRepo::IsConnected() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.isOpen();
}

bool HFRepo::SubscribeToConfigUpdateNotification() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.driver()->subscribeToNotification(CONFIG_UPDATE_NOTIFICATION);
}

bool HFRepo::UnsubscribeFromConfigUpdateNotification() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.driver()->unsubscribeFromNotification(CONFIG_UPDATE_NOTIFICATION);
}

void HFRepo::Notification(const QString& channel, QSqlDriver::NotificationSource source, const QVariant &message) {
    (void)channel;
    (void)source;
    bool ok = false;
    const int id = message.toInt(&ok);
    if (ok) {
        emit SIGNAL_NewConfigurationAdded(id);
    } else {
        qWarning() << "HFRepo::Notification Could not convert the notification message to an integer: " << message.toString();
    }
}

}
