#include "TestRadiationMonitorRepo.h"

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

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

TestRadiationMonitorRepo::TestRadiationMonitorRepo()
    : name_("hardwaredatabase_test"), repo_("_test") {
}

TestRadiationMonitorRepo::~TestRadiationMonitorRepo() {
}

void TestRadiationMonitorRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createRadiationMonitorTables.sql");
    if (!ok1) {
        QFAIL("TestRadiationMonitorRepo::initTestCase Failed loading createRadiationMonitorTables");
    }
    bool ok2 = LoadSqlResourceFile(":/sql/fillRadiationMonitorTestData.sql");
    if (!ok2) {
        QFAIL("TestRadiationMonitorRepo::initTestCase Failed loading fillRadiationMonitorTestData");
    }
}

void TestRadiationMonitorRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestRadiationMonitorRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestRadiationMonitorRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestRadiationMonitorRepo::init() {

}

void TestRadiationMonitorRepo::cleanup() {

}

bool TestRadiationMonitorRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestRadiationMonitorRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestRadiationMonitorRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}

void TestRadiationMonitorRepo::TestGetConfiguration() {
    try {
        QString name = "balise_salle_rd";
        hardware::radiationmonitoring::Configuration act_config =  repo_.GetConfiguration(name);
        hardware::radiationmonitoring::Configuration exp_config(QDateTime(QDate(2017, 12, 12)), name, "192.168.0.1",
                                                                1400, 1000, 2000, 3000, 0, 1, 1, true, false, false, true, true,
                                                                true, 1, 2, 60, 24, 0, 4.32E7, 1.1E10, 1.0E-15, 1.1E-15, 5.79E-13, 1.85E-12);

        QCOMPARE(act_config, exp_config);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetNSingleNames");
    }
}

void TestRadiationMonitorRepo::TestGetRadiationMonitorNames() {
    try {
        std::vector<QString> exp_names = { "balise_salle_rd" };
        auto act_names = repo_.GetRadiationMonitorNames();
        QCOMPARE(act_names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetRadiationMonitorNames");
    }
}

}
