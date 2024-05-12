#include "TestNSingleUtil.h"

#include "NSingleUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {
namespace util {

void TestNSingleUtil::initTestCase() {}

void TestNSingleUtil::cleanupTestCase() {}

void TestNSingleUtil::init() {}

void TestNSingleUtil::cleanup() {}

void TestNSingleUtil::TestCheckSum() {
    QCOMPARE(CheckSum("$A=F0"), QString("2E"));
    QCOMPARE(CheckSum("0"), QString("30"));
    QCOMPARE(CheckSum("00"), QString("00"));
    QCOMPARE(CheckSum("000"), QString("30"));
    QCOMPARE(CheckSum("!ÿ"), QString("DE"));
}

}
}
