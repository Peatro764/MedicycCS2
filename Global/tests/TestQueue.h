#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::global {

class TestQueue : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestEmptyQueue();
    void TestSingleEntry();
    void TestMultipleEntry();
    void TestAddQueue();

private:
};

}
