#include "TestMultiplexConfig.h"
#include "MultiplexConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestMultiplexConfig::initTestCase() {}

void TestMultiplexConfig::cleanupTestCase() {}

void TestMultiplexConfig::init() {}

void TestMultiplexConfig::cleanup() {}

void TestMultiplexConfig::TestEquality() {
    MultiplexConfig c0;
    MultiplexConfig c1;
    c1.AddChannel("t1", 1);
    MultiplexConfig c2;
    c2.AddChannel("t1", 1);
    c2.AddChannel("t2", 2);

    QCOMPARE(c0, c0);
    QCOMPARE(c1, c1);
    QCOMPARE(c2, c2);
    QVERIFY(c1 != c2);
    QVERIFY(c1 != c0);
}

void TestMultiplexConfig::TestSelectChannel() {
    QMap<QString, int> map;
    map["S1"] = 3;
    map["S2"] = 0;
    map["S3"] = 2;
    map["S4"] = 10;
    MultiplexConfig config(map);

    std::bitset<8> bitset1(3);
    QCOMPARE(config.SelectChannel("S1"), bitset1);

    std::bitset<8> bitset2(0);
    QCOMPARE(config.SelectChannel("S2"), bitset2);

    std::bitset<8> bitset3(2);
    QCOMPARE(config.SelectChannel("S3"), bitset3);

    std::bitset<8> bitset4(10);
    QCOMPARE(config.SelectChannel("S4"), bitset4);

    try {
        config.SelectChannel("S5");
        QFAIL("Should have thrown 1");
    }
    catch (std::exception& exc) {}
}

void TestMultiplexConfig::TestName() {
    QMap<QString, int> map;
    map["S1"] = 3;
    map["S2"] = 0;
    map["S3"] = 2;
    map["S4"] = 10;
    MultiplexConfig config(map);

    QCOMPARE(config.Name(2), QString("S3"));
    QCOMPARE(config.Name(0), QString("S2"));
    QCOMPARE(config.Name(10), QString("S4"));
    QCOMPARE(config.Name(3), QString("S1"));
}

void TestMultiplexConfig::TestIterate() {
    QMap<QString, int> map;
    map["S1"] = 3;
    map["S2"] = 0;
    map["S3"] = 2;
    MultiplexConfig config(map);

    QStringList iterated1;
    iterated1.append(config.Iterate());
    iterated1.append(config.Iterate());
    iterated1.append(config.Iterate());
    QVERIFY(iterated1.contains(QString("S1")));
    QVERIFY(iterated1.contains(QString("S2")));
    QVERIFY(iterated1.contains(QString("S3")));

    QStringList iterated2;
    iterated2.append(config.Iterate());
    iterated2.append(config.Iterate());
    iterated2.append(config.Iterate());
    QVERIFY(iterated2.contains(QString("S1")));
    QVERIFY(iterated2.contains(QString("S2")));
    QVERIFY(iterated2.contains(QString("S3")));

    MultiplexConfig config2;
    config2.AddChannel("S1", 1);
    QCOMPARE(config2.Iterate(), QString("S1"));

    config2.AddChannel("S2", 1);
    QStringList iterated3;
    iterated3.append(config2.Iterate());
    iterated3.append(config2.Iterate());
    QVERIFY(iterated3.contains(QString("S1")));
    QVERIFY(iterated3.contains(QString("S2")));
}


}
