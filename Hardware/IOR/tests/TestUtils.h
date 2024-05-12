#ifndef IOR_TESTUTILS_H
#define IOR_TESTUTILS_H

#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class TestUtils : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestHash();

private:
};

}

#endif
