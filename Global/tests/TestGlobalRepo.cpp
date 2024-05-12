#include "TestGlobalRepo.h"
#include "GlobalRepo.h"

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

namespace medicyc::cyclotroncontrolsystem::global {

TestGlobalRepo::TestGlobalRepo()
    : name_("hardwaredatabase_test"), repo_("_test") {
}

TestGlobalRepo::~TestGlobalRepo() {
}

void TestGlobalRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createEnums.sql");
    if (!ok1) {
        QFAIL("TestGlobalRepo::initTestCase Failed loading createEnums");
    }

    bool ok2 = LoadSqlResourceFile(":/sql/createCycloConfigTables.sql");
    if (!ok2) {
        QFAIL("TestGlobalRepo::initTestCase Failed loading createCycloConfigTables");
    }

    bool ok3 = LoadSqlResourceFile(":/sql/fillCycloConfigTestData.sql");
    if (!ok3) {
        QFAIL("TestGlobalRepo::initTestCase Failed loading fillCycloConfigTestData");
    }
}

void TestGlobalRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestGlobalRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestGlobalRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestGlobalRepo::init() {

}

void TestGlobalRepo::cleanup() {

}

bool TestGlobalRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestGlobalRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestGlobalRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}

void TestGlobalRepo::TestGetAvailableConfigs() {
    try {
        QMap<QDateTime, QStringList> result = repo_.GetAvailableConfigs();
        QCOMPARE(result.size(), 3);
        QCOMPARE(result.values().at(0).count("Proton-1") , 1);
        QCOMPARE(result.values().at(0).count("Proton-2") , 1);
        QCOMPARE(result.values().at(0).count("Proton-3") , 1);
        QCOMPARE(result.value(QDateTime(QDate(2019, 4, 8))).at(0), QString("Proton-1"));
        QCOMPARE(result.value(QDateTime(QDate(2020, 4, 8))).at(0), QString("Proton-2"));
        QCOMPARE(result.value(QDateTime(QDate(2018, 4, 8))).at(0), QString("Proton-3"));
    }  catch(...) {
        QFAIL("An exception was thrown in the test TestGetAvailableConfigs");
    }
}

void TestGlobalRepo::TestGetActiveConfig() {
    try {
        QCOMPARE(repo_.GetActiveConfig("VDF"), "Proton-1");
        QCOMPARE(repo_.GetActiveConfig("INJECTION"), "Proton-2");
        try {
            repo_.GetActiveConfig("BLABLA");
            QFAIL("Should have thrown");
        } catch(std::exception& exc){}
    }  catch(...) {
        QFAIL("An exception was thrown in the test TestGetAvailableConfigs");
    }
}

void TestGlobalRepo::TestSetActiveConfig() {
    try {
        repo_.SetActiveConfig("VDF", "Proton-2");
        QCOMPARE(repo_.GetActiveConfig("VDF"), "Proton-2");
        try {
            repo_.SetActiveConfig("BLABLA", "Proton-2");
            repo_.SetActiveConfig("VDF", "Proton-666");
            QFAIL("Should have thrown");
        } catch(std::exception& exc){}
    }  catch(...) {
        QFAIL("An exception was thrown in the test TestSetActiveConfig");
    }
}







}
