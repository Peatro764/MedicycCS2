#include "TestCycloConfig.h"
#include "CycloConfig.h"

#include <QSettings>
#include <QStandardPaths>
#include <QSqlError>
#include <QSignalSpy>
#include <thread>
#include <chrono>

#include "Parameters.h"

namespace cyclo_config {

TestCycloConfig::TestCycloConfig() {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    DatabaseConfig config;
    config.host_name = settings.value(db_name_ + "/host", "unknown").toString();
    config.database_name = settings.value(db_name_ + "/name", "unknown").toString();
    config.port = settings.value(db_name_ + "/port", "0").toInt();
    config.username = settings.value(db_name_ + "/user", "unknown").toString();
    config.password = settings.value(db_name_ + "/password", "unknown").toString();
    config.connection_timeout = 1;

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", "UnitTest");
    db.setHostName(config.host_name);
    db.setPort(config.port);
    db.setDatabaseName(config.database_name);
    db.setUserName(config.username);
    db.setPassword(config.password);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(config.connection_timeout));
    if (!db.open()) {
        QFAIL("Failed opening unittest database");
    }
}

TestCycloConfig::~TestCycloConfig() {
    QSqlDatabase::removeDatabase("UnitTest");
}

bool TestCycloConfig::VerifyCount(QSignalSpy& spy, int expected_count, int timeout) {
    for (int i = 0; i < timeout / 100; ++i) {
        QTest::qWait(100);
        if (spy.count() == expected_count) return true;
    }
    return false;
}

bool TestCycloConfig::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database("UnitTest");
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestCycloConfig::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    return query.exec(cmd);
}

void TestCycloConfig::initTestCase() {
    if (!LoadSqlResourceFile(":/sql/createCycloConfigTables.sql")) {
        QFAIL("TestCycloConfig::initTestCase Failed createCycloConfigTables");
    }

    if (!LoadSqlResourceFile(":/sql/fillTestData.sql")) {
        QFAIL("TestCycloConfig::initTestCase Failed fillTestData");
    }
}

void TestCycloConfig::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database("UnitTest");
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestCycloConfig::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestCycloConfig::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestCycloConfig::init() {}

void TestCycloConfig::cleanup() {}

void TestCycloConfig::TestLastConfig() {
    CycloConfig config(db_name_);
    QSignalSpy spy(&config, &CycloConfig::SIGNAL_LastConfig);
    QVERIFY(spy.isValid());
    spy.wait(100); // to let thread startup
    config.GetLastConfig();
    QVERIFY(VerifyCount(spy, 1, 100));
    QString result = qvariant_cast<QString>(spy.at(0).at(0));
    QCOMPARE(result, QString("Proton-2"));
}

void TestCycloConfig::TestGetConfig_SingleQuery() {
    CycloConfig config(db_name_);
    QSignalSpy spy(&config, &CycloConfig::SIGNAL_ConfigResult);
    QVERIFY(spy.isValid());
    spy.wait(100); // to let thread startup
    config.GetConfig("Proton-2", "VDF");
    QVERIFY(VerifyCount(spy, 1, 100));
    QMap<QString, EquipmentConfig> result = qvariant_cast<QMap<QString, EquipmentConfig>>(spy.at(0).at(0));
    QCOMPARE(result.size(), 3);
    QCOMPARE(result.keys().count("Dipole_M1"), 1);
    QCOMPARE(result.keys().count("Dipole_M2"), 1);
    QCOMPARE(result.keys().count("Dipole_M4"), 1);
    QCOMPARE(result.value("Dipole_M1"), EquipmentConfig("Dipole_M1", 28.0, false, false));
    QCOMPARE(result.value("Dipole_M2"), EquipmentConfig("Dipole_M2", 8.44, false, true));
    QCOMPARE(result.value("Dipole_M4"), EquipmentConfig("Dipole_M4", 38.0, true, true));
}

void TestCycloConfig::TestGetConfig_MultiQuery() {
    CycloConfig config(db_name_);
    QSignalSpy spy(&config, &CycloConfig::SIGNAL_ConfigResult);
    QVERIFY(spy.isValid());
    spy.wait(100); // to let thread startup
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-1", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");
    config.GetConfig("Proton-2", "VDF");

    QVERIFY(VerifyCount(spy, 10, 500));

    // Proton-1
    QMap<QString, EquipmentConfig> result_p1 = qvariant_cast<QMap<QString, EquipmentConfig>>(spy.at(6).at(0));
    QCOMPARE(result_p1.size(), 2);
    QCOMPARE(result_p1.keys().count("Dipole_M1"), 1);
    QCOMPARE(result_p1.value("Dipole_M1"), EquipmentConfig("Dipole_M1", 13.0, true, true));

    // Proton-2
    QMap<QString, EquipmentConfig> result_p2 = qvariant_cast<QMap<QString, EquipmentConfig>>(spy.at(7).at(0));
    QCOMPARE(result_p2.size(), 3);
    QCOMPARE(result_p2.keys().count("Dipole_M1"), 1);
    QCOMPARE(result_p2.keys().count("Dipole_M2"), 1);
    QCOMPARE(result_p2.keys().count("Dipole_M4"), 1);
    QCOMPARE(result_p2.value("Dipole_M1"), EquipmentConfig("Dipole_M1", 28.0, false, false));
    QCOMPARE(result_p2.value("Dipole_M2"), EquipmentConfig("Dipole_M2", 8.44, false, true));
    QCOMPARE(result_p2.value("Dipole_M4"), EquipmentConfig("Dipole_M4", 38.0, true, true));
}

void TestCycloConfig::TestGetBPLevels() {
    CycloConfig config(db_name_);
    QSignalSpy spy(&config, &CycloConfig::SIGNAL_BPLevels);
    QVERIFY(spy.isValid());
    spy.wait(100); // to let thread startup
    config.GetBPLevels("Proton-2");

    QVERIFY(VerifyCount(spy, 1, 500));

    std::vector<BPLevel> result = qvariant_cast<std::vector<BPLevel>>(spy.at(0).at(0));
    QCOMPARE(result.size(), static_cast<size_t>(3));
    QCOMPARE(result.at(0).duration, 40);
    QCOMPARE(result.at(1).duration, 30);
    QCOMPARE(result.at(2).duration, 60);
    QVERIFY(std::abs(result.at(0).setpoint - 1190) < 0.001);
    QVERIFY(std::abs(result.at(1).setpoint - 1310) < 0.001);
    QVERIFY(std::abs(result.at(2).setpoint - 1290) < 0.001);
}

}
