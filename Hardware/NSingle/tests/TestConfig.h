#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestConfig : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestConstructor();
    void TestKinds();
    void TestKind();
    void TestPolarity();
    void TestFiltering();
    void TestAveraging();
    void TestStatusMonitoring();
    void TestKindToString();
    void TestSetKind();
    void TestSetPolarity();
    void TestSetFiltering();
    void TestSetAveraging();
    void TestSetStateMonitoring();

    void TestHexCode();
    void TestBitSet();

private:
};

}
