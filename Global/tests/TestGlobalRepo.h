#ifndef TESTGLOBALREPO_H
#define TESTGLOBALREPO_H

#include <QtTest/QtTest>

#include "GlobalRepo.h"

namespace medicyc::cyclotroncontrolsystem::global {

class TestGlobalRepo : public QObject {
    Q_OBJECT
public:
    TestGlobalRepo();
    ~TestGlobalRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetAvailableConfigs();
    void TestGetActiveConfig();
    void TestSetActiveConfig();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    GlobalRepo repo_;
};

}

#endif
