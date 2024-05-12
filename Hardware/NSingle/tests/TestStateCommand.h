#include <QtTest/QtTest>

#include "StateCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestStateCommand : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestSteadyStateCommandsOnDifferentBits();
    void TestSteadyStateCommandsOneDisabled();
    void TestSteadyStateCommandsOnSameBits();
    void TestImpulsCommands();
    void TestImpulsCommandsNegativeLogic();

private:
};

}
