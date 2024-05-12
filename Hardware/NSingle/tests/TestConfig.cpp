#include "TestConfig.h"
#include "Config.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestConfig::initTestCase() {}

void TestConfig::cleanupTestCase() {}

void TestConfig::init() {}

void TestConfig::cleanup() {}

void TestConfig::TestConstructor() {
    Config config(Config::Kind::BP,
                  Config::Polarity::Bi,
                  Config::Filtering::Low,
                  Config::Averaging::SignedValues,
                  Config::StateMonitoring::OFF);
    QCOMPARE(config.kind(), Config::Kind::BP);
    QCOMPARE(config.polarity(), Config::Polarity::Bi);
    QCOMPARE(config.filtering(), Config::Filtering::Low);
    QCOMPARE(config.averaging(), Config::Averaging::SignedValues);
    QCOMPARE(config.statemonitoring(), Config::StateMonitoring::OFF);
}

void TestConfig::TestKinds() {
    Config config(QString("A1"));
    QMap<QString, Config::Kind> kinds = config.kinds();
    QCOMPARE(kinds.size(), 4);
}

void TestConfig::TestKind() {
    Config config0(QString("A0"));
    QCOMPARE(config0.kind(), Config::Kind::Basic);

    Config config1(QString("A1"));
    QCOMPARE(config1.kind(), Config::Kind::BP);

    Config config2(QString("A2"));
    QCOMPARE(config2.kind(), Config::Kind::BC_ISR);

    Config config3(QString("A4"));
    QCOMPARE(config3.kind(), Config::Kind::VDF);

    Config config4(QString("AC"));
    QCOMPARE(config4.kind(), Config::Kind::VDF);
}

void TestConfig::TestPolarity() {
    Config config0(QString("A0"));
    QCOMPARE(config0.polarity(), Config::Polarity::Uni);

    Config config1(QString("A8"));
    QCOMPARE(config1.polarity(), Config::Polarity::Bi);
}

void TestConfig::TestFiltering() {
    Config config0(QString("0F"));
    QCOMPARE(config0.filtering(), Config::Filtering::None);

    Config config1(QString("1F"));
    QCOMPARE(config1.filtering(), Config::Filtering::Low);

    Config config2(QString("2F"));
    QCOMPARE(config2.filtering(), Config::Filtering::Medium);

    Config config3(QString("3F"));
    QCOMPARE(config3.filtering(), Config::Filtering::High);
}

void TestConfig::TestAveraging() {
    Config config0(QString("0F"));
    QCOMPARE(config0.averaging(), Config::Averaging::AbsoluteValues);

    Config config1(QString("4F"));
    QCOMPARE(config1.averaging(), Config::Averaging::SignedValues);
}

void TestConfig::TestStatusMonitoring() {
    Config config0(QString("0F"));
    QCOMPARE(config0.statemonitoring(), Config::StateMonitoring::OFF);

    Config config1(QString("8F"));
    QCOMPARE(config1.statemonitoring(), Config::StateMonitoring::ON);
}

void TestConfig::TestKindToString() {
    QCOMPARE(Config::KindToString(Config::Kind::Basic), QString("Basic"));
    QCOMPARE(Config::KindToString(Config::Kind::BP), QString("BP"));
    QCOMPARE(Config::KindToString(Config::Kind::BC_ISR), QString("BC_ISR"));
    QCOMPARE(Config::KindToString(Config::Kind::VDF), QString("VDF"));
}

void TestConfig::TestSetKind() {
    Config config1(QString("A0"));
    QCOMPARE(config1.kind(), Config::Kind::Basic);

    config1.SetKind(Config::Kind::BP);
    QCOMPARE(config1.kind(), Config::Kind::BP);

    config1.SetKind(Config::Kind::Basic);
    QCOMPARE(config1.hexcode(), QString("A0"));
}

void TestConfig::TestSetPolarity() {
    Config config1(QString("A8"));
    QCOMPARE(config1.polarity(), Config::Polarity::Bi);

    config1.SetPolarity(Config::Polarity::Uni);
    QCOMPARE(config1.polarity(), Config::Polarity::Uni);
}

void TestConfig::TestSetFiltering() {
    Config config1(QString("18"));
    QCOMPARE(config1.filtering(), Config::Filtering::Low);

    config1.SetFiltering(Config::Filtering::High);
    QCOMPARE(config1.filtering(), Config::Filtering::High);
}

void TestConfig::TestSetAveraging() {
    Config config1(QString("08"));
    QCOMPARE(config1.averaging(), Config::Averaging::AbsoluteValues);

    config1.SetAveraging(Config::Averaging::SignedValues);
    QCOMPARE(config1.averaging(), Config::Averaging::SignedValues);
}

void TestConfig::TestSetStateMonitoring() {
    Config config1(QString("80"));
    QCOMPARE(config1.statemonitoring(), Config::StateMonitoring::ON);

    config1.SetStateMonitoring(Config::StateMonitoring::OFF);
    QCOMPARE(config1.statemonitoring(), Config::StateMonitoring::OFF);
}

void TestConfig::TestHexCode() {
    Config config1(QString("A0"));
    QCOMPARE(config1.hexcode(), QString("A0"));

    config1.SetKind(Config::Kind::BP);
    QCOMPARE(config1.hexcode(), QString("A1"));
}

void TestConfig::TestBitSet() {
    Config config1(QString("A0"));
    std::bitset<8> set1 (160);
    QCOMPARE(config1.bitset(), set1);

    config1.SetKind(Config::Kind::BP);
    std::bitset<8> set2 (161);
    QCOMPARE(config1.bitset(), set2);
}

}
