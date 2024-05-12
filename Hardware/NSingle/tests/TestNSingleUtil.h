#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {
namespace util {

class TestNSingleUtil : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestCheckSum();

private:
};

}
}
