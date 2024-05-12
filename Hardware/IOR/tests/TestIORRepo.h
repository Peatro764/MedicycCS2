#ifndef TESTIORREPO_H
#define TESTIORREPO_H

#include <QtTest/QtTest>

#include "IORRepo.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class TestIORRepo : public QObject {
    Q_OBJECT
public:
    TestIORRepo();
    ~TestIORRepo();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestGetIORChannelNames();
    void TestGetIORChannel();

private:
    bool LoadSqlResourceFile(QString file);

    QString name_;
    IORRepo repo_;
};

}
#endif
