#include "TestPowerSupplyRepo.h"

#include <iostream>
#include <QtDebug>
#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDateTime>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>
#include <map>
#include <QStringList>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

TestPowerSupplyRepo::TestPowerSupplyRepo()
    : name_("hardwaredatabase_test"), repo_("_test") {
}

TestPowerSupplyRepo::~TestPowerSupplyRepo() {
}

void TestPowerSupplyRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createEnums.sql");
    if (!ok1) {
        QFAIL("TestPowerSupplyRepo::initTestCase Failed loading createEnums");
    }

    bool ok2 = LoadSqlResourceFile(":/sql/createCycloConfigTables.sql");
    if (!ok2) {
        QFAIL("TestPowerSupplyRepo::initTestCase Failed loading createCycloConfig");
    }

    bool ok3 = LoadSqlResourceFile(":/sql/createPowerSupplyTables.sql");
    if (!ok3) {
        QFAIL("TestPowerSupplyRepo::initTestCase Failed loading createPowerSupplyTables");
    }

    bool ok4 = LoadSqlResourceFile(":/sql/fillTestData.sql");
    if (!ok4) {
        QFAIL("TestPowerSupplyRepo::initTestCase Failed loading fillTestData");
    }
}

void TestPowerSupplyRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestPowerSupplyRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestPowerSupplyRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestPowerSupplyRepo::init() {

}

void TestPowerSupplyRepo::cleanup() {

}

bool TestPowerSupplyRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestPowerSupplyRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestPowerSupplyRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}


void TestPowerSupplyRepo::TestGetPowerSupplies() {
    try {
        QStringList names =  repo_.GetPowerSupplies();
        QStringList exp_names { "Arc", "Extraction", "Filament" };
        QCOMPARE(names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetPowerSupplies");
    }
}

void TestPowerSupplyRepo::TestGetDbusAddress() {
    try {
        PowerSupplyDbusAddress dbus = repo_.GetDbusAddress("Extraction");
        PowerSupplyDbusAddress exp("Extraction", "medicyc.cyclotron.hardware.sourcepowersupply.extraction", "/Extraction");
        QCOMPARE(dbus, exp);
        try {
            repo_.GetDbusAddress("UNKNOWNSUBSYSTEM");
            QFAIL("Should have thrown");
        } catch(...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetDbusAddress");
    }

}

void TestPowerSupplyRepo::TestGetConfig()  {
    try {
        PowerSupplyConfig config = repo_.GetConfig("Extraction");
        PowerSupplyConfig exp("Extraction", "V.Extraction Source", 1000,
                              RegulatedParameterConfig(0.3, 0.5, 0.3, 0.0, 4.5, 4),
                              RegulatedParameterConfig(0.3, 0.5, 0.3, 0.0, 4.5, 4),
                              "kV", "mA");
        QCOMPARE(config, exp);
        try {
            repo_.GetConfig("Labldld");
            QFAIL("Should have thrown");
        } catch(...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetDbusAddress");
    }
}

void TestPowerSupplyRepo::TestGetSetPoint()  {
    try {
        double v_setpoint = repo_.GetSetPoint("Arc", "V", "VoieMedicale-1");
        double v_exp(160.0);
        QVERIFY(std::abs(v_setpoint - v_exp) < 0.00001);

        double i_setpoint = repo_.GetSetPoint("Arc", "I", "VoieMedicale-1");
        double i_exp(1.0);
        QVERIFY(std::abs(i_setpoint - i_exp) < 0.00001);

        try {
            repo_.GetSetPoint("Arc", "R", "VoieMedicale-1");
            QFAIL("Should have thrown");
        } catch(...){}

        try {
            repo_.GetSetPoint("Blupp", "V", "VoieMedicale-1");
            QFAIL("Should have thrown");
        } catch(...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetSetPoint");
    }
}


}
