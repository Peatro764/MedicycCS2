#include "TestOmronRepo.h"

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

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

TestOmronRepo::TestOmronRepo()
  : name_("hardwaredatabase_test"), repo_("_test") {
  repo_.Connect(1);
}

TestOmronRepo::~TestOmronRepo() {
    repo_.Disconnect();
}

void TestOmronRepo::initTestCase() {
    bool ok1 = LoadSqlResourceFile(":/sql/createOmronTables.sql");
    bool ok2 = LoadSqlResourceFile(":/sql/fillTestData.sql");

    if (!ok1) {
        QFAIL("TestOmronRepo::initTestCase Failed loading createOmronTables");
    }
    if (!ok2) {
        QFAIL("TestOmronRepo::initTestCase Failed loading fillTestData");
    }
}

void TestOmronRepo::cleanupTestCase() {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    if (!query.exec("drop schema public cascade;")) {
        qWarning() << "TestOmronRepo::cleanupTestCase Query drop schema failed" << query.lastError();
    }
    query.clear();

    if (!query.exec("create schema public;")) {
        qWarning() << "TestOmronRepo::cleanupTestCase Query create schema public failed" << query.lastError();
    }
    query.clear();
}

void TestOmronRepo::init() {

}

void TestOmronRepo::cleanup() {

}

bool TestOmronRepo::LoadSqlResourceFile(QString file) {
    QSqlDatabase db = QSqlDatabase::database(name_);
    QSqlQuery query(db);

    QFile sqlData(file);
    if (!sqlData.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "TestOmronRepo::LoadSqlResourceFile Could not open file: " + file;
        return false;
    }
    QTextStream in(&sqlData);
    QString cmd(in.readAll());
    qDebug() << "TestOmronRepo::LoadSqlResourceFile Executing command" << cmd;
    return query.exec(cmd);
}

void TestOmronRepo::TestGetOmronChannelNames() {
    try {
        QStringList names =  repo_.GetOmronChannelNames();
        QStringList exp_names { "HF1 ON", "HF2 ON" };
        QCOMPARE(names, exp_names);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetOmronChannelNames");
    }


}

void TestOmronRepo::TestGetOmronChannel() {
    try {
        omron::Channel ch1 = repo_.GetOmronChannel("HF1 ON");
        QCOMPARE(ch1, omron::Channel("HF1 ON", "HF", 3, omron::FINS_MEMORY_AREA::CIO, 144, 13, true));
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetOmronChannel");
    }
}

void TestOmronRepo::TestGetChannels() {
    try {
        QMap<QString, omron::Channel> channels = repo_.GetChannels("HF");
        omron::Channel ch1("HF1 ON", "HF", 3, omron::FINS_MEMORY_AREA::CIO, 144, 13, true);
        omron::Channel ch2("HF2 ON", "HF", 3, omron::FINS_MEMORY_AREA::CIO, 144, 14, true);
        QMap<QString, omron::Channel> exp_channels;
        exp_channels[ch1.name()] = ch1;
        exp_channels[ch2.name()] = ch2;
        QCOMPARE(exp_channels, channels);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetOmronChannel");
    }
}

void TestOmronRepo::TestGetBarcoMessages() {
    try {
        QMap<int, BarcoMessage> expected_messages;
        BarcoMessage m1(33, BarcoMessage::SEVERITY::WARNING, "05-Defaut Temperature CHF", false);
        BarcoMessage m2(34, BarcoMessage::SEVERITY::INFO, "26-OK FILAMENTS HF1-HF2", true);
        BarcoMessage m3(35, BarcoMessage::SEVERITY::ERROR, "13-PANNE FILAMENT PREAMPLI HF1", true);
        expected_messages[33] = m1;
        expected_messages[34] = m2;
        expected_messages[35] = m3;
        QMap<int, BarcoMessage> actual_messages = repo_.GetBarcoMessages();
        QCOMPARE(expected_messages, actual_messages);
    }
    catch(...) {
        QFAIL("An exception was thrown in the test GetBarcoMessages");
    }
}



}

