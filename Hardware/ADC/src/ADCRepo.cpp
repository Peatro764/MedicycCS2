#include "ADCRepo.h"

#include <QStandardPaths>
#include <QSqlError>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

ADCRepo::ADCRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "ADCRepo::ADCRepo Host " << hostName
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

ADCRepo::~ADCRepo() {
    Disconnect();
}

bool ADCRepo::Connect(int timeout) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
    if (db.open()) {
        qDebug() << "ADCRepo::Connect Connection to repo established";
        return true;
    } else {
        qCritical() << QString("ADCRepo::Connect Failed: ") + db.lastError().text();
        return false;
    }
}

void ADCRepo::Disconnect() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.close();
}

bool ADCRepo::IsConnected() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.isOpen();
}

QStringList ADCRepo::GetADCChannelNames() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM ADCChannel");

    if (!query.exec()) {
        qWarning() << "ADCRepo::GetADCChannelNames Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for ADC channel names failed");
    }

    QStringList channel_names;
    while (query.next()) {
        channel_names.push_back(query.value(0).toString());
    }
    return channel_names;
}

adc::Channel ADCRepo::GetADCChannel(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name, card, address, conversion_factor, unit, sim_value, sim_step "
                  "FROM ADCChannel WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("ADCRepo::GetADCChannel Query failed: ") << query.lastQuery();
        throw std::runtime_error("Db query for ADC channel failed");
    }

    if (query.size() > 1) {
        qWarning() << "ADCRepo::GetADCChannel Multiple ADCs was found having name: " << name;
        throw std::runtime_error("Multiple ADCs with the same name was found");
    }

    if (!query.next()) {
        qWarning() << QString("ADCRepo::GetADCChannel No ADC found with name : ") << name;
        throw std::runtime_error("No ADC with the given name was found");
    }

    return adc::Channel(query.value(0).toString(),
                        query.value(1).toInt(),
                        query.value(2).toInt(),
                        query.value(3).toDouble(),
                        query.value(4).toString(),
                        query.value(5).toDouble(),
                        query.value(6).toDouble());
}

}
