#include "TestRegulatedParameter.h"
#include "RegulatedParameter.h"

#include <QSignalSpy>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

NSingleConfig TestRegulatedParameter::GetNSingleConfig(int setpoint_lsb, int setpoint_msb, int value_lsb, int value_msb, int signal_variation, int buffer_size) {
    return NSingleConfig("nsinglename", "ip", 3000,
                  MeasurementConfig(setpoint_lsb, setpoint_msb, 15, 0.1),
                  MeasurementConfig(value_lsb, value_msb, 15, 0.1),
                  StateReplyConfig(Bit(), Bit(), Bit(), Bit(), Bit(), Bit(), Bit(), Bit(), Bit()),
                  StateCommandConfig(true, Bit(), Bit(), Bit(), Bit()),
                  Config("00"), true, false,
                         5, 4,
                  true, false, true, false, true, false,
                  signal_variation, 30.0,
                         1000, 2000, buffer_size, 2,
                         100, 101, 102, 103, 104, 1000, 20, 2, true);
}

bool TestRegulatedParameter::VerifyCount(QSignalSpy& spy, int expected_count, int timeout) {
    for (int i = 0; i < timeout / 100; ++i) {
        QTest::qWait(100);
        if (spy.count() == expected_count) return true;
    }
    return false;
}

void TestRegulatedParameter::initTestCase() {}

void TestRegulatedParameter::cleanupTestCase() {}

void TestRegulatedParameter::init() {}

void TestRegulatedParameter::cleanup() {}

void TestRegulatedParameter::SignalCharacteristics() {
    auto config = GetNSingleConfig(0, 14, 0, 14, 1, 4);
    RegulatedParameter r(config);

    Measurement m(config.GetChannel1ValueConfig());
    m.SetData(5, true);
    r.ActValueReceived(m);
    QVERIFY(std::abs(r.Mean() - 5.0) < 0.0001);
    QVERIFY(r.StdDev() < 0.0001);
    QVERIFY(r.StdErr() < 0.0001);

    m.SetData(10, true);
    r.ActValueReceived(m);
    QVERIFY(std::abs(r.Mean() - 7.5) < 0.001);
    QVERIFY(std::abs(r.StdDev() - 2.5) < 0.001);
    QVERIFY(std::abs(r.StdErr() - 1.7677) < 0.001);

    m.SetData(15, true);
    r.ActValueReceived(m);
    QVERIFY(std::abs(r.Mean() - 10.0) < 0.001);
    QVERIFY(std::abs(r.StdDev() - 4.082) < 0.001);
    QVERIFY(std::abs(r.StdErr() - 2.357) < 0.001);

    m.SetData(20, true);
    r.ActValueReceived(m);
    QVERIFY(std::abs(r.Mean() - 12.5) < 0.001);
    QVERIFY(std::abs(r.StdDev() - 5.590) < 0.001);
    QVERIFY(std::abs(r.StdErr() - 2.795) < 0.001);

    m.SetData(25, true);
    r.ActValueReceived(m);
    QVERIFY(std::abs(r.Mean() - 17.5) < 0.001);
    QVERIFY(std::abs(r.StdDev() - 5.590) < 0.001);
    QVERIFY(std::abs(r.StdErr() - 2.795) < 0.001);
}

void TestRegulatedParameter::NearTarget() {
    auto config = GetNSingleConfig(0, 14, 0, 14, 1, 4);
    RegulatedParameter r(config);

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(30, true);
    Measurement m_act1(config.GetChannel1ValueConfig());
    m_act1.SetData(30 + 0x1, true);
    Measurement m_act2(config.GetChannel1ValueConfig());
    m_act2.SetData(30 + 0x6, true);

    r.SetDesValue(m_des);

    r.ActValueReceived(m_act1);
    QVERIFY(r.NearTarget());

    r.ActValueReceived(m_act2);
    QVERIFY(!r.NearTarget());
}

void TestRegulatedParameter::NearZero() {
    auto config = GetNSingleConfig(0, 14, 0, 14, 1, 4);
    RegulatedParameter r(config);

    Measurement m_act1(config.GetChannel1ValueConfig());
    m_act1.SetData(0x1, true);
    r.ActValueReceived(m_act1);
    QVERIFY(r.NearZero());

    Measurement m_act2(config.GetChannel1ValueConfig());
    m_act2.SetData(0x5, true);
    r.ActValueReceived(m_act2);
    QVERIFY(!r.NearZero());
}

void TestRegulatedParameter::SignalSteady() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 5);
    RegulatedParameter r(config);

    Measurement m(config.GetChannel1ValueConfig());
    m.SetData(30, true);
    r.ActValueReceived(m);
    m.SetData(30, true);
    r.ActValueReceived(m);
    m.SetData(30, true);
    r.ActValueReceived(m);
    m.SetData(29, true);
    r.ActValueReceived(m);
    m.SetData(29, true);
    r.ActValueReceived(m);
    QVERIFY(r.SignalSteady());

    m.SetData(26, true);
    r.ActValueReceived(m);
    QVERIFY(!r.SignalSteady());
}

void TestRegulatedParameter::PolarityCorrect() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 5);
    RegulatedParameter r(config);

    Measurement m(config.GetChannel1ValueConfig());
    m.SetData(30, false);
    r.ActValueReceived(m);
    QVERIFY(!r.PolarityCorrect());
}

void TestRegulatedParameter::OnTarget_VaryMean() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 5);
    RegulatedParameter r(config);

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(30, true);
    r.SetDesValue(m_des);

    Measurement m_act(config.GetChannel1ValueConfig());
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(29, true);
    r.ActValueReceived(m_act);
    m_act.SetData(29, true);
    r.ActValueReceived(m_act);
    QVERIFY(r.OnTarget());

    m_act.SetData(29, true);
    r.ActValueReceived(m_act);
    QVERIFY(!r.OnTarget());
}

void TestRegulatedParameter::OnTarget_VaryStdDev() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 4);
    RegulatedParameter r(config);

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(30, true);
    r.SetDesValue(m_des);

    Measurement m_act(config.GetChannel1ValueConfig());
    m_act.SetData(32, true);
    r.ActValueReceived(m_act);
    m_act.SetData(32, true);
    r.ActValueReceived(m_act);
    m_act.SetData(28, true);
    r.ActValueReceived(m_act);
    m_act.SetData(28, true);
    r.ActValueReceived(m_act);
    QVERIFY(!r.OnTarget());
}

void TestRegulatedParameter::SignalIsEmittedWhenDesiredValueIsUpdated() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 4);
    RegulatedParameter r(config);

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(30, true);

    QSignalSpy spy(&r, &RegulatedParameter::SIGNAL_DesUpdated);
    QVERIFY(spy.isValid());

    r.SetDesValue(m_des);
    QVERIFY(VerifyCount(spy, 1, 100));
    double res1 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol1 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_des.InterpretedValue() - res1) < 0.001);
    QVERIFY(pol1);
    spy.clear();

    r.SetDesPhysicalValue(m_des.InterpretedValue());
    QVERIFY(VerifyCount(spy, 1, 100));
    double res2 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol2 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_des.InterpretedValue() - res2) < 0.001);
    QVERIFY(pol2);
    spy.clear();

    r.SetDesPhysicalValue(m_des.InterpretedValue(), true);
    QVERIFY(VerifyCount(spy, 1, 100));
    double res3 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol3 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_des.InterpretedValue() - res3) < 0.001);
    QVERIFY(pol3);
    spy.clear();

    r.SetDesPhysicalValue(m_des.InterpretedValue(), false);
    QVERIFY(VerifyCount(spy, 1, 100));
    double res4 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol4 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(!pol4);
    QVERIFY(std::abs(m_des.InterpretedValue() - res4) < 0.001);
    spy.clear();

    Measurement m_act(config.GetChannel1ValueConfig());
    m_act.SetData(32, true);
    r.ActValueReceived(m_act);
    r.SetActAsDesValue();
    QVERIFY(VerifyCount(spy, 1, 100));
    double res5 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol5 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_act.InterpretedValue() - res5) < 0.001);
    QVERIFY(pol5);
    spy.clear();

    r.SetDesValue(m_des);
    QVERIFY(VerifyCount(spy, 1, 100));
    spy.clear();

    m_des.Increment(1);
    r.IncrementDesValue(1);
    QVERIFY(VerifyCount(spy, 1, 100));
    double res6 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol6 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_des.InterpretedValue() - res6) < 0.001);
    QVERIFY(pol6);
    spy.clear();

    m_des.Decrement(1);
    r.DecrementDesValue(1);
    QVERIFY(VerifyCount(spy, 1, 100));
    double res7 = qvariant_cast<double>(spy.at(0).at(0));
    bool pol7 = qvariant_cast<bool>(spy.at(0).at(1));
    QVERIFY(std::abs(m_des.InterpretedValue() - res7) < 0.001);
    QVERIFY(pol7);
    spy.clear();
}

void TestRegulatedParameter::TestGetImprovedSetPoint() {
    int signal_variation = 1;
    auto config = GetNSingleConfig(0, 14, 0, 14, signal_variation, 5);
    RegulatedParameter r(config);

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(30, true);
    r.SetDesValue(m_des);

    Measurement m_set(config.GetChannel1SetPointConfig());
    m_set.SetData(30, true);
    r.SetPointReceived(m_set);

    Measurement m_act(config.GetChannel1ValueConfig());
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(30, true);
    r.ActValueReceived(m_act);
    m_act.SetData(29, true);
    r.ActValueReceived(m_act);
    m_act.SetData(29, true);
    r.ActValueReceived(m_act);

    QCOMPARE(r.GetSetPoint(), m_set);

    m_act.SetData(29, true);
    r.ActValueReceived(m_act);
    m_set.Increment(1);
    r.SetImprovedSetPoint();
    QCOMPARE(r.GetSetPoint(), m_set);
}

void TestRegulatedParameter::MeasurementContainerDoesNotStartAtBit0() {
    int signal_variation = 16;
    auto config = GetNSingleConfig(2, 14, 3, 14, signal_variation, 5);
    RegulatedParameter r(config);

    Measurement m_act(config.GetChannel1ValueConfig());
    m_act.SetData(40, true);
    r.ActValueReceived(m_act);
    m_act.SetData(40, true);
    r.ActValueReceived(m_act);
    m_act.SetData(40, true);
    r.ActValueReceived(m_act);
    m_act.SetData(24, true);
    r.ActValueReceived(m_act);
    m_act.SetData(24, true);
    r.ActValueReceived(m_act);
    QVERIFY(r.SignalSteady());

    Measurement m_des(config.GetChannel1ValueConfig());
    m_des.SetData(32, true);
    r.SetDesValue(m_des);
    QVERIFY(r.OnTarget());

    m_des.SetData(24, true);
    r.SetDesValue(m_des);
    QVERIFY(!r.OnTarget());

    m_act.SetData(48, true);
    r.ActValueReceived(m_act);
    QVERIFY(!r.SignalSteady());
}


}
