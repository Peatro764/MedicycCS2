#include "OmronRepo.h"

#include <QtDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <stdexcept>
#include <QSqlDriver>
#include <QMetaMethod>
#include <QStandardPaths>
#include <QSqlError>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

OmronRepo::OmronRepo(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {

    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    QString hostName = settings.value(db_name_ + "/host", "unknown").toString();
    QString databaseName = settings.value(db_name_ + "/name", "unknown").toString();
    int port = settings.value(db_name_ + "/port", "0").toInt();
    QString userName = settings.value(db_name_ + "/user", "unknown").toString();
    QString password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "OmronRepo::OmronRepo Host " << hostName
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

OmronRepo::~OmronRepo() {
    Disconnect();
}

bool OmronRepo::Connect(int timeout) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(timeout));
    if (db.open()) {
        qDebug() << "OmronRepo::Connect Connection to repo established";
        return true;
    } else {
        qCritical() << QString("OmronRepo::Connect Failed: ") + db.lastError().text();
        return false;
    }
}

void OmronRepo::Disconnect() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    db.close();
}

bool OmronRepo::IsConnected() {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    return db.isOpen();
}

QStringList OmronRepo::GetOmronSystemNames() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM OmronSystem");

    if (!query.exec()) {
        qWarning() << "OmronRepo::GetOmronSystemNames Query failed: " << query.lastError();
        throw std::runtime_error("Db query for Omron system names failed");
    }

    QStringList system_names;
    while (query.next()) {
        system_names.push_back(query.value(0).toString());
    }
    return system_names;
}

QStringList OmronRepo::GetOmronChannelNames() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name FROM OmronChannel");

    if (!query.exec()) {
        qWarning() << "OmronRepo::GetOmronChannelNames Query failed: " << query.lastError();
        throw std::runtime_error("Db query for Omron channel names failed");
    }

    QStringList channel_names;
    while (query.next()) {
        channel_names.push_back(query.value(0).toString());
    }
    return channel_names;
}

omron::Channel OmronRepo::GetOmronChannel(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT name, system, node, type, address, bit, impuls "
                  "FROM OmronChannel WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("OmronRepo::GetOmronChannel Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for Omron channel failed");
    }

    if (query.size() > 1) {
        qWarning() << "Repository::GetOmronChannel Multiple omron channales was found having name: " << name;
        throw std::runtime_error("Multiple omron channels with the same name was found");
    }

    if (!query.next()) {
        qWarning() << QString("Repository::GetOmronChannel No omron channels found with name : ") << name;
        throw std::runtime_error("No omron channel with the given name was found");
    }

    const int node = query.value(2).toInt();
    if (node < 0 || node > 0xFF) {
        throw std::runtime_error("Node must be between 0 and 0xFF");
    }

    const int address = query.value(4).toInt();
    if (address < 0 || address > 0xFFFF) {
        throw std::runtime_error("Address must be between 0 and 0xFFFF");
    }

    const int bit = query.value(5).toInt();
    if (bit < 0 || bit > 15) {
        throw std::runtime_error("Bit channel must be between 0 and 15");
    }

    return omron::Channel(query.value(0).toString(),
                        query.value(1).toString(),
                        static_cast<uint8_t>(node),
                        omron::ToFinsMemoryArea(query.value(3).toString()),
                        static_cast<uint16_t>(address),
                        static_cast<uint8_t>(bit),
                        query.value(6).toBool());
}

QMap<QString, omron::Channel> OmronRepo::GetChannels(QString system) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    if (system == QString("ALL")) {
        query.prepare("SELECT name, system, node, type, address, bit, impuls "
                      "FROM OmronChannel ORDER BY node, address, bit ASC");
    } else {
        query.prepare("SELECT name, system, node, type, address, bit, impuls "
                      "FROM OmronChannel WHERE system = :system ORDER BY node, address, bit ASC");
        query.bindValue(":system", system);
    }
    if (!query.exec()) {
        qWarning() << QString("OmronRepo::GetChannels Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for Omron channels failed");
    }

    if (query.size() < 1) {
        qWarning() << "Repository::GetChannels Empty set";
        throw std::runtime_error("No omron channels found");
    }

    QMap<QString, omron::Channel> channels;
    while(query.next()) {
        const int node = query.value(2).toInt();
        if (node < 0 || node > 0xFF) {
            throw std::runtime_error("Node must be between 0 and 0xFF");
        }

        const int address = query.value(4).toInt();
        if (address < 0 || address > 0xFFFF) {
            throw std::runtime_error("Address must be between 0 and 0xFFFF");
        }

        const int bit = query.value(5).toInt();
        if (bit < 0 || bit > 15) {
            throw std::runtime_error("Bit channel must be between 0 and 15");
        }

        Channel ch(query.value(0).toString(),
                   query.value(1).toString(),
                   static_cast<uint8_t>(node),
                   omron::ToFinsMemoryArea(query.value(3).toString()),
                   static_cast<uint16_t>(address),
                   static_cast<uint8_t>(bit),
                   query.value(6).toBool());
        channels[ch.name()] = ch;
    }
    return channels;
}

QMap<int, BarcoMessage> OmronRepo::GetBarcoMessages() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT id, message, enabled, severity  "
                  "FROM BarcoMessage");

    if (!query.exec()) {
        qWarning() << QString("OmronRepo::GetBarcoMessages Query failed: ") << query.lastError();
        throw std::runtime_error("Db query for Barco messages failed");
    }

    if (query.size() < 1) {
        qWarning() << "OmronRepo::GetBarcoMessages Empty set";
        throw std::runtime_error("No barco messages found");
    }

    QMap<int, BarcoMessage> messages;
    while(query.next()) {
        const int id = query.value(0).toInt();
        const QString message = query.value(1).toString();
        const bool enabled = query.value(2).toBool();
        const BarcoMessage::SEVERITY severity = static_cast<BarcoMessage::SEVERITY>(query.value(3).toInt());
        BarcoMessage msg(id,
                         severity,
                         message,
                         enabled);
        messages[id] = msg;
    }
    return messages;
}

}
