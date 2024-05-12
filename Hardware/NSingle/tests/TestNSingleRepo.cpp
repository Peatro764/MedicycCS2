#include "TestNSingleRepo.h"
#include "NSingleRepo.h"

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

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

TestNSingleRepo::TestNSingleRepo()
    : name_("hardwaredatabase_test"), repo_("_test") {
}

TestNSingleRepo::~TestNSingleRepo() {
}

void TestNSingleRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createEnums.sql");
    if (!ok1) {
        QFAIL("TestNSingleRepo::initTestCase Failed loading createEnums");
    }

    bool ok2 = LoadSqlResourceFile(":/sql/createCycloConfigTables.sql");
    if (!ok2) {
        QFAIL("TestNSingleRepo::initTestCase Failed loading createCycloConfigTables");
    }

    bool ok3 = LoadSqlResourceFile(":/sql/createNSingleTables.sql");
    if (!ok3) {
        QFAIL("TestNSingleRepo::initTestCase Failed loading createNSingleTables");
    }

    bool ok4 = LoadSqlResourceFile(":/sql/fillNSingleTestData.sql");
    if (!ok4) {
        QFAIL("TestNSingleRepo::initTestCase Failed loading fillNSingleTestData");
    }
}

void TestNSingleRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestNSingleRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestNSingleRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestNSingleRepo::init() {

}

void TestNSingleRepo::cleanup() {

}

bool TestNSingleRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestNSingleRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestNSingleRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}


void TestNSingleRepo::TestGetHardNSingles() {
    try {
        QStringList names =  repo_.GetHardNSingles();
        QStringList exp_names { "Bobine_de_Correction_C09", "Bobine_Principale", "Multiplexed_1" };
        QCOMPARE(names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetNSingleNames");
    }
}

void TestNSingleRepo::TestGetSubSystemNSingles() {
    try {
        std::vector<QString> names_1 =  repo_.GetNSingles("BOBINESDECORRECTION");
        std::vector<QString> exp_names_1 { "Bobine_de_Correction_C00", "Bobine_de_Correction_C01", "Bobine_de_Correction_C09" };
        QCOMPARE(names_1, exp_names_1);

        std::vector<QString> names_2 =  repo_.GetNSingles("BOBINEPRINCIPALE");
        std::vector<QString> exp_names_2 { "Bobine_Principale" };
        QCOMPARE(names_2, exp_names_2);

        try {
            repo_.GetNSingles("UNKNOWNSUBSYSTEM");
            QFAIL("Should have thrown");
        } catch(...){}
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetSubSystemNSingles");
    }

}

void TestNSingleRepo::TestGetAllSubSystemNSingles() {
    try {
        QMap<QString, bool> act_nsingles_1 =  repo_.GetAllNSingles("BOBINESDECORRECTION");
        QMap<QString, bool> exp_nsingles_1;
        exp_nsingles_1["Bobine_de_Correction_C00"] = true;
        exp_nsingles_1["Bobine_de_Correction_C01"] = true;
        exp_nsingles_1["Bobine_de_Correction_C02"] = false;
        exp_nsingles_1["Bobine_de_Correction_C09"] = true;
        QCOMPARE(act_nsingles_1, exp_nsingles_1);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetUnAvailableNSingles");
    }
}


void TestNSingleRepo::TestGetSubSystemsNSingles() {
    try {
        QStringList sub_systems_1 {"BOBINESDECORRECTION", "BOBINEPRINCIPALE"};
        std::vector<QString> names_1 =  repo_.GetNSingles(sub_systems_1);
        std::vector<QString> exp_names_1 {"Bobine_de_Correction_C00", "Bobine_de_Correction_C01", "Bobine_de_Correction_C09", "Bobine_Principale" };
        QCOMPARE(names_1, exp_names_1);

        QStringList sub_systems_2 {"BOBINEPRINCIPALE"};
        std::vector<QString> names_2 =  repo_.GetNSingles(sub_systems_2);
        std::vector<QString> exp_names_2 { "Bobine_Principale" };
        QCOMPARE(names_2, exp_names_2);

        try {
            QStringList sub_systems_3 {"UNKNOWN"};
            repo_.GetNSingles(sub_systems_3);
            QFAIL("Should have thrown");
        } catch(...){}

        QStringList sub_systems_4;
        std::vector<QString> names_4 = repo_.GetNSingles(sub_systems_4);
        std::vector<QString> exp_names_4;
        QCOMPARE(names_4, exp_names_4);
    } catch(...) {
        QFAIL("An exception was thrown in the test GetSubSystemsNSingles");
    }
}

void TestNSingleRepo::TestGetNSinglesInConfiguration() {
    QMap<QString, bool> nsingles1 = repo_.GetNSingles("BOBINESDECORRECTION", "Proton-1");
    QCOMPARE(nsingles1.size(), 3);
    QVERIFY(nsingles1.contains("Bobine_de_Correction_C00"));
    QVERIFY(nsingles1.contains("Bobine_de_Correction_C01"));
    QVERIFY(nsingles1.contains("Bobine_de_Correction_C09"));
    QVERIFY(nsingles1["Bobine_de_Correction_C00"]);
    QVERIFY(nsingles1["Bobine_de_Correction_C01"]);
    QVERIFY(nsingles1["Bobine_de_Correction_C09"]);

    QMap<QString, bool> nsingles2 = repo_.GetNSingles("VDF", "Proton-1");
    QVERIFY(nsingles2.empty());

    auto nsingles3 = repo_.GetNSingles("BOBINESDECORRECTION", "Proton-3");
    QCOMPARE(nsingles3.size(), 1);
    QVERIFY(nsingles3.contains("Bobine_de_Correction_C09"));

    auto nsingles4 = repo_.GetNSingles("BOBINESDECORRECTION", "Proton-11");
    QVERIFY(nsingles4.empty());
}

void TestNSingleRepo::TestGetNSingleShortName() {
    QCOMPARE(repo_.GetNSingleShortName("Bobine_Principale"), QString("BP"));
    try {
        repo_.GetNSingleShortName("DDDKD");
        QFAIL("Should have thrown");
    } catch(...){}
}

void TestNSingleRepo::TestGetNSingleConfig() {
    try {
        hw_nsingle::NSingleConfig bp_config = repo_.GetNSingleConfig("Bobine_Principale");

        QString ip("172.20.216.131");
        int port(1470);
        hw_nsingle::MeasurementConfig setpoint_config(0, 14, 15, 0.0367779);
        hw_nsingle::MeasurementConfig value_config(1, 13, 14, 0.05);
        hw_nsingle::StateReplyConfig state_reply_config(hw_nsingle::Bit(true, true, 1),
                                                     hw_nsingle::Bit(true, true, 0),
                                                     hw_nsingle::Bit(true, true, 2),
                                                     hw_nsingle::Bit(true, true, 3),
                                                     hw_nsingle::Bit(true, false, 3),
                                                     hw_nsingle::Bit(true, true, 4),
                                                     hw_nsingle::Bit(true, true, 5),
                                                     hw_nsingle::Bit(false, false, 0),
                                                     hw_nsingle::Bit(false, false, 0));
        hw_nsingle::StateCommandConfig state_command_config(true,
                                                         hw_nsingle::Bit(false, false, 0),
                                                         hw_nsingle::Bit(true, true, 0),
                                                         hw_nsingle::Bit(true, true, 2),
                                                         hw_nsingle::Bit(true, true, 3));
        hw_nsingle::Config low_level_config(hw_nsingle::Config::Kind::BP,
                                         hw_nsingle::Config::Polarity::Uni,
                                         hw_nsingle::Config::Filtering::Low,
                                         hw_nsingle::Config::Averaging::AbsoluteValues,
                                         hw_nsingle::Config::StateMonitoring::ON);
        hw_nsingle::NSingleConfig exp_config("Bobine_Principale", ip, port,
                                          setpoint_config, value_config,
                                          state_reply_config, state_command_config,
                                          low_level_config,
                                          false, false,
                                          100, 30,
                                          false, false, false, true,true,false,
                                          14,
                                          1200, 1000, 3000, 6, 2,
                                          30000, 30000, 220000, 40000, 220000, 1000, 20, 2, true);
        QCOMPARE(bp_config.GetChannel1SetPointConfig(), setpoint_config);
        QCOMPARE(bp_config.GetChannel1ValueConfig(), value_config);
        QCOMPARE(bp_config.GetStateCommandConfig(), state_command_config);
        QCOMPARE(bp_config.GetStateReplyConfig(), state_reply_config);
        QCOMPARE(bp_config.GetLowLevelConfig(), low_level_config);

        QCOMPARE(bp_config, exp_config);
    }
    catch(std::exception& exc) {
        QString msg = QString("An exception was thrown in the test GetNSingleConfig: ") + QString::fromStdString(exc.what());
        QFAIL(msg.toStdString().c_str());
    }
}

void TestNSingleRepo::TestGetMultiplexConfig() {
    try {
        hw_nsingle::MultiplexConfig exp_config;
        exp_config.AddChannel("Bobine_de_Correction_C00", 1);
        exp_config.AddChannel("Bobine_de_Correction_C01", 2);
        hw_nsingle::MultiplexConfig config = repo_.GetMultiplexConfig("Multiplexed_1");
        QCOMPARE(config, exp_config);
    }
    catch(std::exception& exc) {
        QString msg = QString("An exception was thrown in the test GetMultiplexConfig: ") + QString::fromStdString(exc.what());
        QFAIL(msg.toStdString().c_str());
    }
}

void TestNSingleRepo::TestGetNSingleSetPoint() {
    try {
        hw_nsingle::NSingleSetPoint setpoint1 = repo_.GetNSingleSetPoint("Proton-2", "Bobine_de_Correction_C00");
        hw_nsingle::NSingleSetPoint exp_setpoint1(true, false, 28);
        QCOMPARE(setpoint1, exp_setpoint1);

        hw_nsingle::NSingleSetPoint setpoint2 = repo_.GetNSingleSetPoint("Proton-2", "Bobine_de_Correction_C09");
        hw_nsingle::NSingleSetPoint exp_setpoint2(true, true, 38);
        QCOMPARE(setpoint2, exp_setpoint2);

        hw_nsingle::NSingleSetPoint setpoint3 = repo_.GetNSingleSetPoint("Proton-2", "Bobine_de_Correction_C02");
        hw_nsingle::NSingleSetPoint exp_setpoint3(false, false, 0);
        QCOMPARE(setpoint2, exp_setpoint2);
    } catch (std::exception& exc) {
        QString msg = QString("An exception was thrown in the test TestGetNSingleSetpoint: ") + QString::fromStdString(exc.what());
        QFAIL(msg.toStdString().c_str());
    }

    try {
        repo_.GetNSingleSetPoint("Proton-2", "Bobine_de_Correction_C12");
        QFAIL("TestGetNSingleSetPoint1: Should have thrown");
    } catch (std::exception& exc) {}

    try {
        repo_.GetNSingleSetPoint("Proton-20", "Bobine_de_Correction_C02");
        QFAIL("TestGetNSingleSetPoint2: Should have thrown");
    } catch (std::exception& exc) {}

}

}
