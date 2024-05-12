#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestError : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestTypes();
    void TestTypeToString();
    void TestHexCode();
    void TestBitSet();
    void TestActive();

private:
};

}
