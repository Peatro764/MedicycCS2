#ifndef TESTCHANNEL_H
#define TESTCHANNEL_H

#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class TestChannel : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestComparison();

private:
};

}

#endif
