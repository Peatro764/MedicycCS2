#include "TestChannel.h"
#include "ADC.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

void TestChannel::initTestCase() {}

void TestChannel::cleanupTestCase() {}

void TestChannel::init() {}

void TestChannel::cleanup() {}

void TestChannel::TestComparison() {
    Channel ch1("a", 0, 1, 1.0, "mA", 0, 0);
    Channel ch2("b", 0, 1, 1.0, "mA", 0, 0);
    Channel ch3("a", 1, 1, 1.0, "mA", 0, 0);
    Channel ch4("a", 0, 2, 1.0, "mA", 0, 0);
    Channel ch5("a", 0, 1, 2.0, "mA", 0, 0);
    Channel ch6("a", 0, 1, 2.0, "V", 0, 0);
    QVERIFY(ch1 == ch1);
    QVERIFY(ch1 != ch2);
    QVERIFY(ch1 != ch3);
    QVERIFY(ch1 != ch4);
    QVERIFY(ch1 != ch5);
    QVERIFY(ch1 != ch6);
}

}
