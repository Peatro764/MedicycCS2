#ifndef TESTRADIATIONMONITORREPO_H
#define TESTRADIATIONMONITORREPO_H

#include <QtTest/QtTest>

#include "RadiationMonitorRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class TestRadiationMonitorRepo : public QObject {
    Q_OBJECT
public:
    TestRadiationMonitorRepo();
    ~TestRadiationMonitorRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetConfiguration();
    void TestGetRadiationMonitorNames();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    RadiationMonitorRepo repo_;
};

}

#endif
