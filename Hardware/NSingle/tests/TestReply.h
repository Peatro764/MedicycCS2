#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestReply : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestIsValid();
    void TestHeader();
    void TestType();
    void TestBody();
    void TestFooter();
    void TestCheckSum();

private:
};

}
