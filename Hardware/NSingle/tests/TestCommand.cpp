#include "TestCommand.h"
#include "Command.h"
#include "NSingleUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestCommand::initTestCase() {}

void TestCommand::cleanupTestCase() {}

void TestCommand::init() {}

void TestCommand::cleanup() {}

void TestCommand::TestPackaged() {
    QCOMPARE(Command("H", "1023", "RR").Packaged(), QString("H1023") + util::CheckSum(QString("H1023")) + "RR");
}

}
