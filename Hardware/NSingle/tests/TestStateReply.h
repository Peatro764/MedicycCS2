#include <QtTest/QtTest>

#include "StateReply.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestStateReply : public QObject {
    Q_OBJECT
private slots:
    StateReplyConfig GetStateReplyConfig();
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestSetGetData();
    void TestChannel();

private:
};

}
