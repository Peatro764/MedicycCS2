#include <QtTest/QtTest>

#include "FINS.h"
#include "NodeAddressCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class TestNodeAddressCommand : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestNodeAddressRequest();
    void TestNodeAddressReply();

private:
};

}
