#include "TestADCRepo.h"
#include "ADCRepo.h"

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

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

TestADCRepo::TestADCRepo()
    : name_("testdatabase"), repo_("testdatabase") {
    repo_.Connect(1);
}

TestADCRepo::~TestADCRepo() {
    repo_.Disconnect();
}

void TestADCRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createADCTables.sql");
    bool ok2 = LoadSqlResourceFile(":/sql/fillTestData.sql");

    if (ok1 && ok2) {
        qDebug() << "TestADCRepo::initTestCase Test database initialised";
    } else {
        QFAIL("TestADCRepo::initTestCase Failed initialising test database");
    }
}

void TestADCRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestADCRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestADCRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestADCRepo::init() {

}

void TestADCRepo::cleanup() {

}

bool TestADCRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestADCRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestADCRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}



void TestADCRepo::TestGetADCChannelNames() {
    try {
        QStringList names =  repo_.GetADCChannelNames();
        QStringList exp_names { "SP-D1", "SP-D2" };
        QCOMPARE(names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetADCChannelNames");
    }

}

void TestADCRepo::TestGetADCChannel() {
    try {
        adc::Channel sp_d1 = repo_.GetADCChannel("SP-D1");
        QCOMPARE(sp_d1, Channel("SP-D1", 0, 4, 1.0, "mA", 0, 0));
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetADCChannel");
    }
}

}
