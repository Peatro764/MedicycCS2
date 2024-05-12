#include "TestMeasurement.h"

#include <iostream>
#include <QDateTime>

#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

void TestMeasurement::initTestCase() {
}

void TestMeasurement::cleanupTestCase() {}

void TestMeasurement::init() {}

void TestMeasurement::cleanup() {}

void TestMeasurement::MeasurementOperators() {
    Measurement m1(QDateTime(QDate(2017, 3, 3)), 10.0, 10.0);
    Measurement m2(QDateTime(QDate(2017, 3, 4)), 10.0, 10.0);

    QVERIFY(m1 == m1);
    QVERIFY(m1 != m2);
    QVERIFY(m1 < m2);
}

void TestMeasurement::IntegratedMeasurementOperators() {
    IntegratedMeasurement m1(QDateTime(QDate(2017, 3, 3)), 10.0, 10.0);
    IntegratedMeasurement m2(QDateTime(QDate(2017, 3, 4)), 10.0, 10.0);

    QVERIFY(m1 == m1);
    QVERIFY(m1 != m2);
    QVERIFY(m1 < m2);
}

void TestMeasurement::IntegratedMeasurementBufferOperators() {
    Measurement m1(QDateTime(QDate(2017, 3, 3)), 10.0, 10.0);
    Measurement m2(QDateTime(QDate(2017, 3, 4)), 10.0, 10.0);
    IntegratedMeasurementBuffer b1(m1, m1);
    IntegratedMeasurementBuffer b2(m1, m2);

    QVERIFY(b1 == b1);
    QVERIFY(b1 != b2);
    QVERIFY(b1 < b2);
}

}


