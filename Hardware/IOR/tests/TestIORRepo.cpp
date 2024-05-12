#include "TestIORRepo.h"

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

#include "IOR.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

TestIORRepo::TestIORRepo()
    : name_("hardwaredatabase_test"), repo_("_test") {
    repo_.Connect(1);
}

TestIORRepo::~TestIORRepo() {
    repo_.Disconnect();
}

void TestIORRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createIORTables.sql");
    bool ok2 = LoadSqlResourceFile(":/sql/fillTestData.sql");

    if (ok1 && ok2) {
        qDebug() << "TestIORRepo::initTestCase Test database initialised";
    } else {
        QFAIL("TestIORRepo::initTestCase Failed initialising test database");
    }
}

void TestIORRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestIORRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestIORRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestIORRepo::init() {

}

void TestIORRepo::cleanup() {

}

bool TestIORRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestIORRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestIORRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}

void TestIORRepo::TestGetIORChannelNames() {
    try {
        QStringList names =  repo_.GetIORChannelNames();
        QStringList exp_names { "CF8", "CF9" };
        QCOMPARE(names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetIORChannelNames");
    }

}

void TestIORRepo::TestGetIORChannel() {
    try {
        ior::Channel cf8 = repo_.GetIORChannel("CF8");
        QCOMPARE(cf8, ior::Channel("CF8", 0, 1, 4));
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetIORChannel");
    }
}

}
