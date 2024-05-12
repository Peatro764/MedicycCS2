#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestMultiplexConfig : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestEquality();
    void TestSelectChannel();
    void TestName();
    void TestIterate();

private:
};

}
