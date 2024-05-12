#include <QtTest>
#include "TestNSingleUtil.h"
#include "TestCommand.h"
#include "TestReply.h"
#include "TestConfig.h"
#include "TestMeasurement.h"
#include "TestError.h"
#include "TestStateReply.h"
#include "TestStateCommand.h"
#include "TestMultiplexConfig.h"
#include "TestRegulatedParameter.h"
#include "TestNSingleRepo.h"

namespace nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

int main(int argc, char** argv) {
//    QApplication app(argc, argv);

    nsingle::util::TestNSingleUtil testNSingleUtil;
    nsingle::TestCommand testCommand;
    nsingle::TestReply testReply;
    nsingle::TestConfig testConfig;
    nsingle::TestMeasurement testMeasurement;
    nsingle::TestError testError;
    nsingle::TestStateReply testStateReply;
    nsingle::TestStateCommand testStateCommand;
    nsingle::TestMultiplexConfig testMultiplexConfig;
    nsingle::TestRegulatedParameter testRegulated;
    nsingle::TestNSingleRepo testRepo;

    return QTest::qExec(&testConfig, argc, argv) |
           QTest::qExec(&testMeasurement, argc, argv) |
            QTest::qExec(&testError, argc, argv) |
            QTest::qExec(&testStateReply, argc, argv) |
           QTest::qExec(&testReply, argc, argv) |
            QTest::qExec(&testNSingleUtil, argc, argv) |
           QTest::qExec(&testCommand, argc, argv) |
            QTest::qExec(&testStateCommand, argc, argv) |
            QTest::qExec(&testMultiplexConfig, argc, argv) |
      QTest::qExec(&testRegulated, argc, argv) |
            QTest::qExec(&testRepo, argc, argv);
}

