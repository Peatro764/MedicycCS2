#include "NSingleRepo.h"

#include <QSettings>
#include <QSqlError>
#include <QDebug>

#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

NSingleRepo::NSingleRepo(QString db_suffix)
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

NSingleRepo::~NSingleRepo() {
    QSqlDatabase::removeDatabase(db_name_);
}

QStringList NSingleRepo::GetHardNSingles() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetHardNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    QString query_string("SELECT name FROM NSingle WHERE available = true");
    query_string.append(" AND multiplexed = false");
    query_string.append(" ORDER BY name ASC");
    query.prepare(query_string);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetHardNSingles Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for nsingle names failed");
    }

    QStringList nsingles;
    while (query.next()) {
        nsingles.push_back(query.value(0).toString());
    }
    return nsingles;
}


std::vector<QString> NSingleRepo::GetNSingles(QStringList sub_systems) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    QString query_string("SELECT name FROM NSingle WHERE available = true");
    // Dont take the multiplexed nsingle itself, only its virtual channels
    query_string.append(" AND name not like 'Multiplexed_%'");
    if (!sub_systems.empty()) {
        for (QString &s : sub_systems) s = s.prepend("'").append("'");
        query_string.append(" AND subsystem in (").append(sub_systems.join(",").append(")"));
    } else {
        qWarning() << "NSingleRepo::GetNSingles stringlist empty, returning null set";
        return std::vector<QString>();
    }
    query_string.append(" ORDER BY name ASC");

    query.prepare(query_string);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetNSingles Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for nsingle names failed");
    }

    std::vector<QString> nsingles;
    while (query.next()) {
        nsingles.push_back(query.value(0).toString());
    }
    return nsingles;
}

std::vector<QString> NSingleRepo::GetNSingles(QString sub_system) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    QString query_string("SELECT name FROM NSingle WHERE available = true");
    // Dont take the multiplexed nsingle itself, only its virtual channels
    query_string.append(" AND name not like 'Multiplexed_%'");
    if (sub_system != QString("ALL")) query_string.append(" AND subsystem = :subsystem");
    query_string.append(" ORDER BY name ASC");
    query.prepare(query_string);

    if (sub_system != QString("ALL")) query.bindValue(":subsystem", sub_system);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetNSingles Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for nsingle names failed");
    }

    std::vector<QString> nsingles;
    while (query.next()) {
        nsingles.push_back(query.value(0).toString());
    }
    return nsingles;
}

QMap<QString, bool> NSingleRepo::GetAllNSingles(QString sub_system) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetUnAvailableNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    QString query_string("SELECT name, available FROM NSingle");
    // Dont take the multiplexed nsingle itself, only its virtual channels
    query_string.append(" WHERE name not like 'Multiplexed_%'");
    query_string.append(" AND subsystem = :subsystem");
    query_string.append(" ORDER BY name ASC");
    query.prepare(query_string);
    query.bindValue(":subsystem", sub_system);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetUnAvailableNSingles Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for nsingle names failed");
    }

    QMap<QString, bool> nsingles;
    while (query.next()) {
        nsingles[query.value(0).toString()] = query.value(1).toBool();
    }
    return nsingles;
}

QString NSingleRepo::GetNSingleShortName(QString nsingle) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT short_name FROM NSingle WHERE name = :name");
    query.bindValue(":name", nsingle);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetNSingleShortName Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for nsingle short name failed");
    }

    if (!query.next()) {
        qWarning() << QString("NSingleNSingleRepo::GetNSingleShortName No short name found for nsingle : ") << nsingle;
        throw std::runtime_error("No short name found for nsingle");
    }

    return query.value(0).toString();
}

QMap<QString, bool> NSingleRepo::GetNSingles(QString sub_system, QString cyclo_config) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingles Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    QString query_string("SELECT ns.name AS name, n.available AS available FROM nsinglesetpoint ns, nsingle n "
                         "WHERE n.name = ns.name AND n.subsystem = :sub_system AND ns.cyclo_config = :cyclo_config "
                         "ORDER BY ns.name ASC");
    query.prepare(query_string);
    query.bindValue(":sub_system", sub_system);
    query.bindValue(":cyclo_config", cyclo_config);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetNSingles Query failed: " << query.lastQuery() << " " << query.lastError();
        throw std::runtime_error("Db query for nsingle names failed");
    }

    // <name, available>
    QMap<QString, bool> nsingles;
    while (query.next()) {
        nsingles[query.value(0).toString()] = query.value(1).toBool();
    }
    return nsingles;
}


hw_nsingle::NSingleConfig NSingleRepo::GetNSingleConfig(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingleConfig Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    query.prepare("SELECT name, class, type, polarity, filtering, averaging, state_monitoring, ip, port, multiplexed, multichannel, setpoint_lsb, setpoint_msb, setpoint_sign_bit, "
                  "setpoint_lsb_conversion, value_lsb, value_msb, value_sign_bit, value_lsb_conversion, near_zero_limit, near_target_limit, "
                  "polarity_change_allowed, hot_polarity_change, "
                  "reset_at_on, switchoff_on_shutdown, verify_setpoints, active_compensation, signal_variation, upper_limit, read_value_interval, "
                  "read_state_interval, regulation_buffer_size, timeout_command_factor, timeout_checkstate, timeout_startup, timeout_roughpositioning, "
                  "timeout_finepositioning, timeout_shutdown, ramp_cmd_inc_time, ramp_max_delta, ramp_power_delta, ramp_activated "
                  "FROM NSingle WHERE name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("NSingleRepo::GetNSingleConfig Query failed: ") << query.lastQuery();
        throw std::runtime_error("Db query for nsingle failed");
    }

    if (query.size() > 1) {
        qWarning() << "NSingleRepo::GetNSingleConfig Multiple nsingles was found having name: " << name;
        throw std::runtime_error("Multiple nsingles with the same name was found");
    }

    if (!query.next()) {
        qWarning() << QString("NSingleRepo::GetNSingleConfig No nsingle found with name : ") << name;
        throw std::runtime_error("No nsingle with the given name was found");
    }

    hw_nsingle::StateReplyConfig state_reply_config = this->GetNSingleStateReplyConfig(name);
    hw_nsingle::StateCommandConfig state_command_config = this->GetNSingleStateCommandConfig(name);
    const hw_nsingle::Config low_level_config(
                hw_nsingle::Config::kinds().value(query.value(2).toString()),
                hw_nsingle::Config::polarities().value(query.value(3).toString()),
                hw_nsingle::Config::filterings().value(query.value(4).toString()),
                hw_nsingle::Config::averagings().value(query.value(5).toString()),
                hw_nsingle::Config::statemonitorings().value(query.value(6).toString()));
    const QString ip = query.value(7).toString();
    const int port = query.value(8).toInt();
    const bool multiplexed = query.value(9).toBool();
    const bool multichannel = query.value(10).toBool();
    hw_nsingle::MeasurementConfig ch1_setpoint(query.value(11).toInt(), // lsb
                                            query.value(12).toInt(), // msb
                                            query.value(13).toInt(), // sign bit
                                            query.value(14).toDouble()); // conversion
    hw_nsingle::MeasurementConfig ch1_value(query.value(15).toInt(), // lsb
                                            query.value(16).toInt(), // msb
                                            query.value(17).toInt(), // sign bit
                                            query.value(18).toDouble()); // conversion

    int near_zero_limit = query.value(19).toInt();
    int near_target_limit = query.value(20).toInt();
    bool polarity_change_allowed = query.value(21).toBool();
    bool hot_polarity_change = query.value(22).toBool();
    bool reset_at_on = query.value(23).toBool();
    bool switchoff_on_shutdown = query.value(24).toBool();
    bool verify_setpoints = query.value(25).toBool();
    bool active_compensation = query.value(26).toBool();
    int signal_variation = query.value(27).toInt();
    double upper_limit = query.value(28).toDouble();
    int read_value_interval = query.value(29).toInt();
    int read_state_interval = query.value(30).toInt();
    int regulation_buffer_size = query.value(31).toInt();
    int timeout_command_factor = query.value(32).toInt();
    int timeout_checkstate = query.value(33).toInt();
    int timeout_startup = query.value(34).toInt();
    int timeout_roughpositioning = query.value(35).toInt();
    int timeout_finepositioning = query.value(36).toInt();
    int timeout_shutdown = query.value(37).toInt();
    double ramp_cmd_inc_time = query.value(38).toDouble();
    double ramp_max_delta = query.value(39).toDouble();
    int ramp_power_delta = query.value(40).toInt();
    bool ramp_activated = query.value(41).toBool();

    return hw_nsingle::NSingleConfig(query.value(0).toString().replace(" ", "_"), ip, port,
                                     ch1_setpoint,
                                     ch1_value,
                                     state_reply_config,
                                     state_command_config,
                                     low_level_config,
                                     multiplexed, multichannel,
                                     near_zero_limit,
                                     near_target_limit,
                                     polarity_change_allowed,
                                     hot_polarity_change,
                                     reset_at_on,
                                     switchoff_on_shutdown,
                                     verify_setpoints,
                                     active_compensation,
                                     signal_variation,
                                     upper_limit,
                                     read_value_interval,
                                     read_state_interval,
                                     regulation_buffer_size,
                                     timeout_command_factor,
                                     timeout_checkstate,
                                     timeout_startup,
                                     timeout_roughpositioning,
                                     timeout_finepositioning,
                                     timeout_shutdown,
                                     ramp_cmd_inc_time,
                                     ramp_max_delta,
                                     ramp_power_delta,
                                     ramp_activated);
}

QStringList NSingleRepo::GetMultiplexedChannels(QString nsingle) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetMultiplexedChannels Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    query.prepare("SELECT m.channel, m.address FROM nsinglemultiplexconfig m, nsingle n "
                  "WHERE m.nsingle_name = :nsingle_name and m.channel = n.name and n.available = true "
                  "ORDER by m.address ASC");
    query.bindValue(":nsingle_name", nsingle);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetMultiplexedChannels Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for multiplexed channels failed");
    }

    QStringList channels;
    while (query.next()) {
        channels.push_back(query.value(0).toString());
    }
    return channels;
}

hw_nsingle::MultiplexConfig  NSingleRepo::GetMultiplexConfig(QString nsingle) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetMultiplexedConfig Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }
    QSqlQuery query(db);
    query.prepare("SELECT m.channel, m.address FROM NSingleMultiplexConfig m, nsingle n "
                  "WHERE m.nsingle_name = :nsingle_name and m.channel = n.name and n.available = true "
                  "ORDER by m.address ASC");
    query.bindValue(":nsingle_name", nsingle);

    if (!query.exec()) {
        qWarning() << QString("NSingleRepo::GetMultiplexConfig Query failed: ") << query.lastQuery();
        throw std::runtime_error("Db query for multiplex config");
    }

    if (query.size() == 0) {
        qWarning() << "NSingleRepo::GetMultiplexConfig non found";
        throw std::runtime_error("No multiplex config found");
    }

    hw_nsingle::MultiplexConfig config;
    while(query.next()) {
        config.AddChannel(query.value(0).toString().replace(" ", "_"), query.value(1).toInt());
    }

    return config;
}

hw_nsingle::StateReplyConfig NSingleRepo::GetNSingleStateReplyConfig(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT stdby, off_, on_, ok, error, local_, remote, switch1, switch2 "
                  "FROM NSingleStateReply WHERE nsingle_name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("NSingleRepo::GetNSingleStateReplyConfig Query failed: ") << query.lastQuery();
        throw std::runtime_error("Db query for nsingle state reply config failed");
    }

    if (query.size() != 1) {
        qWarning() << "NSingleRepo::GetNSingleStateReplyConfig No nsingle state reply configs with name: " << name;
        throw std::runtime_error("No nsingle state reply configs found");
    }

    if (!query.next()) {
        qWarning() << "NSingleRepo::GetNSingleStateReplyConfig No nsingle state reply configs with name: " << name;
        throw std::runtime_error("No nsingle state reply configs found");
    }

    return hw_nsingle::StateReplyConfig(hw_nsingle::Bit(query.value(0).toString().simplified()),
                                hw_nsingle::Bit(query.value(1).toString().simplified()),
                                hw_nsingle::Bit(query.value(2).toString().simplified()),
                                hw_nsingle::Bit(query.value(3).toString().simplified()),
                                hw_nsingle::Bit(query.value(4).toString().simplified()),
                                hw_nsingle::Bit(query.value(5).toString().simplified()),
                                hw_nsingle::Bit(query.value(6).toString().simplified()),
                                hw_nsingle::Bit(query.value(7).toString().simplified()),
                                hw_nsingle::Bit(query.value(8).toString().simplified()));
}

hw_nsingle::StateCommandConfig NSingleRepo::GetNSingleStateCommandConfig(QString name) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    QSqlQuery query(db);
    query.prepare("SELECT impuls, stdby, off_, on_, reset "
                  "FROM NSingleStateCommand WHERE nsingle_name = :name");
    query.bindValue(":name", name);

    if (!query.exec()) {
        qWarning() << QString("NSingleRepo::GetNSingleStateCommandConfig Query failed: ") << query.lastQuery();
        throw std::runtime_error("Db query for nsingle state command config failed");
    }

    if (query.size() != 1) {
        qWarning() << "NSingleRepo::GetNSingleStateCommandConfig No nsingle state command configs with name: " << name;
        throw std::runtime_error("No nsingle state command configs found");
    }

    if (!query.next()) {
        qWarning() << "NSingleRepo::GetNSingleStateCommandConfig No nsingle state command configs with name: " << name;
        throw std::runtime_error("No nsingle state command configs found");
    }

    return hw_nsingle::StateCommandConfig(query.value(0).toBool(),
                                       hw_nsingle::Bit(query.value(1).toString().simplified()),
                                       hw_nsingle::Bit(query.value(2).toString().simplified()),
                                       hw_nsingle::Bit(query.value(3).toString().simplified()),
                                       hw_nsingle::Bit(query.value(4).toString().simplified()));
}

hw_nsingle::NSingleDbusAddress NSingleRepo::GetNSingleDbusAddress(QString nsingle) const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingleDbusAddress Failed open db";
        throw std::runtime_error("Base de données non connectée");
    }

    QSqlQuery query(db);
    query.prepare("SELECT interface_address, controller_object_name, nsingle_object_name FROM NSingleDBusAddress WHERE name = :name");
    query.bindValue(":name", nsingle);

    if (!query.exec()) {
        qWarning() << "NSingleRepo::GetNSingleDbusAddress Query failed: " << query.lastQuery();
        throw std::runtime_error("Db query for nsingle dbus address failed");
    }

    if (!query.next()) {
        qWarning() << QString("NSingleNSingleRepo::GetNSingleDbusAddress No dbus address found for nsingle : ") << nsingle;
        throw std::runtime_error("No dbus address found for nsingle");
    }

    return hw_nsingle::NSingleDbusAddress(nsingle,
                                          query.value(0).toString(),
                                          query.value(1).toString(),
                                          query.value(2).toString());
}

hw_nsingle::NSingleSetPoint NSingleRepo::GetNSingleSetPoint(QString cyclo_config, QString nsingle) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetNSingleSetPoint Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT powered_up, sign, value "
                  "FROM NSingleSetPoint "
                  "WHERE cyclo_config = :cyclo_config AND name = :name");
    query.bindValue(":cyclo_config", cyclo_config);
    query.bindValue(":name", nsingle);

    if (!query.exec()){
        qWarning() << QString("NSingleRepo::GetNSingleSetPoint Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }

    if(!query.next()) {
        qWarning() << QString("NSingleRepo::GetNSingleSetPoint Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }
    hw_nsingle::NSingleSetPoint set_point(query.value(0).toBool(),
                                          query.value(1).toString() == QString("+"),
                                          query.value(2).toDouble());
    return set_point;
}

void NSingleRepo::SaveNSingleSetPoint(QString cyclo_config, QString nsingle, hw_nsingle::NSingleSetPoint set_point) {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::SaveNSingleSetPoint Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO NSingleSetPoint(name, cyclo_config, enabled, sign, value) "
                  "VALUES(:name, :cyclo_config, :enabled, :sign, :value)");
    query.bindValue(":name", nsingle);
    query.bindValue(":cyclo_config", cyclo_config);
    query.bindValue(":powered_up", set_point.powered_up());
    query.bindValue(":sign", set_point.sign() ? QString("+") : QString("-"));
    query.bindValue(":value", set_point.value());

    if (!query.exec()){
        qWarning() << QString("NSingleRepo::SaveNSingleSetPoint Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }
}

std::vector<BPLevel> NSingleRepo::GetBPLevels() const {
    QSqlDatabase db = QSqlDatabase::database(db_name_);
    if (!db.open()) {
        qWarning() << "NSingleRepo::GetBPLevels Failed open";
        throw std::runtime_error("Défaillance de la base de données");
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("SELECT setpoint, duration "
                  "FROM bpcyclelevel "
                  "ORDER BY id ASC");

    if (!query.exec()){
        db.close();
        qWarning() << QString("NSingleRepo::GetBPLevels Query failed:") << query.lastQuery();
        throw std::runtime_error("Défaillance de la base de données");
    }

    std::vector<BPLevel> levels;
    while (query.next()) {
        BPLevel level;
        level.setpoint = query.value(0).toDouble();
        level.duration = query.value(1).toInt();
        levels.push_back(level);
    }
    return levels;
}

}
