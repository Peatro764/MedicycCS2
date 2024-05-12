#ifndef TESTPOWERSUPPLYREPO_H
#define TESTPOWERSUPPLYREPO_H

#include <QtTest/QtTest>

#include "SourcePowerSupplyRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

class TestPowerSupplyRepo : public QObject {
    Q_OBJECT
public:
    TestPowerSupplyRepo();
    ~TestPowerSupplyRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetPowerSupplies();
    void TestGetDbusAddress();
    void TestGetConfig();
    void TestGetSetPoint();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    SourcePowerSupplyRepo repo_;
};

}

#endif
