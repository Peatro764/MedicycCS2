#ifndef TESTADCREPO_H
#define TESTADCREPO_H

#include <QtTest/QtTest>

#include "ADCRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class TestADCRepo : public QObject {
    Q_OBJECT
public:
    TestADCRepo();
    ~TestADCRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetADCChannelNames();
    void TestGetADCChannel();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    ADCRepo repo_;
};

}
#endif
