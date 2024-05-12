#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestCommand : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestPackaged();

private:
};

}
