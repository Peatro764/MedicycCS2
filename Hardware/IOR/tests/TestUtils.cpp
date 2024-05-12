#include "TestUtils.h"
#include "IORUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

void TestUtils::initTestCase() {}

void TestUtils::cleanupTestCase() {}

void TestUtils::init() {}

void TestUtils::cleanup() {}

void TestUtils::TestHash() {
    QCOMPARE(ior_util::Hash(0, 0, 0), 0);
    QCOMPARE(ior_util::Hash(0, 0, 1), 1);
    QCOMPARE(ior_util::Hash(0, 1, 2), 10);
    QCOMPARE(ior_util::Hash(1, 0, 0), 24);
}

}
