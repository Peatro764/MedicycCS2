#include "TestMeasurement.h"
#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestMeasurement::initTestCase() {}

void TestMeasurement::cleanupTestCase() {}

void TestMeasurement::init() {}

void TestMeasurement::cleanup() {}

void TestMeasurement::TestEgality() {
    MeasurementConfig c1(2, 5, 13, 2.0);
    MeasurementConfig c2(0, 5, 13, 2.0);
    Measurement m11(c1);
    Measurement m12(c1);
    Measurement m21(c2);
    Measurement m22(c2);

    // Same config, different data, all data in interval
    m11.SetData(24);
    m12.SetData(20);
    QVERIFY(m11 == m11);
    QVERIFY(m11 != m12);

    // Same config, data in interval same, but bits active outside interval
    m11.SetData(24);
    m12.SetData(25);
    QVERIFY(m11 == m12);

    m11.SetData(24);
    m12.SetData(88);
    QVERIFY(m11 == m12);

    // Different config, same data
    m11.SetData(24);
    m21.SetData(24);
    QVERIFY(m11 != m21);
}

void TestMeasurement::TestCanContainData() {
    MeasurementConfig c1(2, 5, 13, 2.0);
    Measurement m1(c1);
    QVERIFY(m1.CanContainData(63-3));
    QVERIFY(!m1.CanContainData(64-3));
    QVERIFY(m1.CanContainData((63.0-3) * c1.bit_value()));
    QVERIFY(!m1.CanContainData((64.0-3) * c1.bit_value()));
    QVERIFY(m1.CanContainData("003C"));
    QVERIFY(!m1.CanContainData("003D"));
    QVERIFY(m1.CanContainData("203C"));
    QVERIFY(!m1.CanContainData("103C"));
    QVERIFY(!m1.CanContainData("403C"));
}

void TestMeasurement::TestMinMaxValues() {
    MeasurementConfig c1(2, 5, 13, 2.0);
    QCOMPARE(c1.max(), 60);
    QCOMPARE(c1.min(), 4);

    MeasurementConfig c2(0, 3, 13, 2.0);
    QCOMPARE(c2.max(), 15);
    QCOMPARE(c2.min(), 1);
}

void TestMeasurement::TestOneBit() {
    MeasurementConfig c1(2, 5, 13, 2.0);
    Measurement m1(c1);
    QCOMPARE(m1.OneBit(), 4);

    MeasurementConfig c2(0, 3, 13, 2.0);
    Measurement m2(c2);
    QCOMPARE(m2.OneBit(), 1);
}

void TestMeasurement::TestSetHex() {
    // Bit field in the middle, negative sign
    MeasurementConfig c0(3, 12, 13, 2.0);
    Measurement m0(c0);
    m0.SetData(QString("3217"));
    QCOMPARE(m0.RawValue(), 4624);
    QVERIFY(std::abs(m0.InterpretedValue() - 4624*2.0) < 0.001);
    QCOMPARE(m0.HexValue(), QString("3210"));
    QCOMPARE(m0.sign(), false);
    std::bitset<16> bits0(0x3210);
    QCOMPARE(m0.bits(), bits0);

    // Bit field in the middle, positive sign
    MeasurementConfig c1(3, 12, 13, 2.0);
    Measurement m1(c1);
    m1.SetData(QString("1217"));
    QCOMPARE(m1.RawValue(), 4624);
    QVERIFY(std::abs(m1.InterpretedValue() - 4624*2.0) < 0.001);
    QCOMPARE(m1.HexValue(), QString("1210"));
    QCOMPARE(m1.sign(), true);
    std::bitset<16> bits1(0x1210);
    QCOMPARE(m1.bits(), bits1);

    // Bit field in the beginning
    MeasurementConfig c2(0, 9, 10, 2.0);
    Measurement m2(c2);
    m2.SetData(QString("0217"));
    QCOMPARE(m2.RawValue(), 535);
    QVERIFY(std::abs(m2.InterpretedValue() - 535*2.0) < 0.001);
    QCOMPARE(m2.HexValue(), QString("0217"));
    QCOMPARE(m2.sign(), true);
    std::bitset<16> bits2(0x0217);
    QCOMPARE(m2.bits(), bits2);

    // Bit field at the end, positive sign
    MeasurementConfig c3(7, 15, 6, 2.0);
    Measurement m3(c3);
    m3.SetData(QString("F217"));
    QCOMPARE(m3.RawValue(), 61952);
    QVERIFY(std::abs(m3.InterpretedValue() - 61952*2.0) < 0.001);
    QCOMPARE(m3.HexValue(), QString("F200"));
    QCOMPARE(m3.sign(), true);
    std::bitset<16> bits3(0xF200);
    QCOMPARE(m3.bits(), bits3);

    // Bit field at the end, negative sign
    MeasurementConfig c4(7, 15, 6, 2.0);
    Measurement m4(c4);
    m4.SetData(QString("F257"));
    QCOMPARE(m4.RawValue(), 61952);
    QVERIFY(std::abs(m4.InterpretedValue() - 61952*2.0) < 0.001);
    QCOMPARE(m4.HexValue(), QString("F240"));
    QCOMPARE(m4.sign(), false);
    std::bitset<16> bits4(0xF240);
    QCOMPARE(m4.bits(), bits4);
}

void TestMeasurement::TestSetBitField() {
    // Bit field in the middle
    MeasurementConfig c1(3, 12, 13, 2.0);
    Measurement m1(c1);
    std::bitset<16> bits1 (0x1217);
    m1.SetData(bits1);
    QCOMPARE(m1.bits(), bits1);
}

void TestMeasurement::TestSetRawData() {
    MeasurementConfig c1(3, 12, 13, 2.0);
    Measurement m1(c1);
    m1.SetData(578, true);
    QCOMPARE(m1.RawValue(), 576);
    QVERIFY(std::abs(m1.InterpretedValue() - 576*2.0) < 0.001);
    QCOMPARE(m1.HexValue(), QString("0240"));
    QCOMPARE(m1.sign(), true);
    std::bitset<16> bits1(0x0240);
    QCOMPARE(m1.bits(), bits1);


    MeasurementConfig c2(0, 2, 13, 2.0);
    Measurement m2(c2);
    m2.SetData(c2.min() - 1, true);
    QCOMPARE(m2.HexValue(), QString("0000"));
}

void TestMeasurement::TestSetInterpretedData() {
    MeasurementConfig c1(3, 12, 13, 2.0);
    Measurement m1(c1);
    m1.SetData(static_cast<double>(578*2.0), true);
    QCOMPARE(m1.RawValue(), 576);
    QVERIFY(std::abs(m1.InterpretedValue() - 576*2.0) < 0.001);
    QCOMPARE(m1.HexValue(), QString("0240"));
    QCOMPARE(m1.sign(), true);
    std::bitset<16> bits1(0x0240);
    QCOMPARE(m1.bits(), bits1);
}

void TestMeasurement::TestIncrement() {
    // bit 3 -> = 8
    MeasurementConfig c1(3, 11, 12, 2.0);
    Measurement m1(c1);
    m1.SetData(QString("1FE0")); // 1FF8
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("1FE8"));
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("1FF0"));
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("1FF8"));
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("1FF8"));

    MeasurementConfig c2(3, 11, 12, 2.0);
    Measurement m2(c2);
    m1.SetData(QString("0FE0")); // 0FF8
    m1.Increment(2);
    QCOMPARE(m1.HexValue(), QString("0FF0"));
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("0FF8"));
    m1.Increment(1);
    QCOMPARE(m1.HexValue(), QString("0FF8"));
}

void TestMeasurement::TestDecrement() {
    MeasurementConfig c1(2, 5, 13, 2.0);
    Measurement m1(c1);
    m1.SetData(QString("200C"));

    m1.Decrement(1);
    QCOMPARE(m1.HexValue(), QString("2008"));

    m1.Decrement(1);
    QCOMPARE(m1.HexValue(), QString("2004"));
    m1.Decrement(1);
    QCOMPARE(m1.HexValue(), QString("2000"));
    m1.Decrement(1);
    QCOMPARE(m1.HexValue(), QString("2000"));

    MeasurementConfig c2(0, 14, 15, 0.003);
    Measurement m2(c2);
    m2.SetData(QString("8003"));
    m2.Decrement(1);
    QCOMPARE(m2.HexValue(), QString("8002"));
    m2.Decrement(1);
    QCOMPARE(m2.HexValue(), QString("8001"));
    m2.Decrement(1);
    QCOMPARE(m2.HexValue(), QString("8000"));
    m2.Decrement(1);
    QCOMPARE(m2.HexValue(), QString("8000"));

    MeasurementConfig c3(0, 14, 15, 0.003);
    Measurement m3(c3);
    m3.SetData(QString("0003"));
    m3.Decrement(1);
    QCOMPARE(m3.HexValue(), QString("0002"));
    m3.Decrement(1);
    QCOMPARE(m3.HexValue(), QString("0001"));
    m3.Decrement(1);
    QCOMPARE(m3.HexValue(), QString("0000"));
    m3.Decrement(1);
    QCOMPARE(m3.HexValue(), QString("0000"));
}

void TestMeasurement::TestInvertSign() {
    MeasurementConfig c1(2, 5, 13, 2.0);

    Measurement m1(c1);
    m1.SetData(10, true);
    m1.InvertSign();

    Measurement m1_neg(c1);
    m1_neg.SetData(10, false);

    QCOMPARE(m1, m1_neg);

    Measurement m1_pos(c1);
    m1_pos.SetData(10, true);

    m1.InvertSign();
    QCOMPARE(m1, m1_pos);
}


}
