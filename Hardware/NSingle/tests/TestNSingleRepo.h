#ifndef TESTNSINGLEREPO_H
#define TESTNSINGLEREPO_H

#include <QtTest/QtTest>

#include "NSingleRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestNSingleRepo : public QObject {
    Q_OBJECT
public:
    TestNSingleRepo();
    ~TestNSingleRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetHardNSingles();
    void TestGetSubSystemNSingles();
    void TestGetAllSubSystemNSingles();
    void TestGetSubSystemsNSingles();
    void TestGetNSinglesInConfiguration();
    void TestGetNSingleShortName();
    void TestGetNSingleConfig();
    void TestGetMultiplexConfig();
    void TestGetNSingleSetPoint();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    NSingleRepo repo_;
};

}

#endif
