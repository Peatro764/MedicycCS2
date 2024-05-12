#include "TestXRayIdentifier.h"

#include <iostream>
#include <QDateTime>

#include "XRayIdentifier.h"
#include "Measurement.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

void TestXRayIdentifier::initTestCase() {
}

void TestXRayIdentifier::cleanupTestCase() {}

void TestXRayIdentifier::init() {}

void TestXRayIdentifier::cleanup() {}

void TestXRayIdentifier::SingleEntryInInterval() {
    bool xray = false;
    XRayIdentifier identifier(true, 1, 3, 10.0, 100.0);
    QObject::connect(&identifier, &XRayIdentifier::XRayIdentified, this, [&xray]() { xray = true; });

    // simplest xray case
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    xray = false;

    // upper to lower flank
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 150.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.0));
    QVERIFY(!xray);

    // lower to upper flank
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 120.0));
    QVERIFY(!xray);

    // reset to zero
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.0));

    // xray less simple 1
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    xray = false;
}

void TestXRayIdentifier::MultipleEntryInInterval() {
    bool xray = false;
    XRayIdentifier identifier(true, 1, 3, 10.0, 100.0);
    QObject::connect(&identifier, &XRayIdentifier::XRayIdentified, this, [&xray]() { xray = true; });

    // double
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    xray = false;

    // triple
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    xray = false;

    // four -> should not be identified as an xray
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(!xray);
}

void TestXRayIdentifier::PeakValue() {
    bool xray = false;
    XRayIdentifier identifier(true, 1, 3, 10.0, 100.0);
    InstantenousMeasurement det_peak;
    QObject::connect(&identifier, &XRayIdentifier::XRayIdentified, this,
                     [&xray, &det_peak](InstantenousMeasurement m) { xray = true; det_peak = m; });
    InstantenousMeasurement real_peak(QDateTime(QDate(2017, 1, 1)), 0.0, 90.0);

    // single
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(real_peak);
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    QCOMPARE(det_peak, real_peak);
    xray = false;

    // double
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(real_peak);
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    QCOMPARE(det_peak, real_peak);
    xray = false;

    // triple
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 0.1));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    identifier.AddInstantaneousValue(real_peak);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 50.0));
    QVERIFY(!xray);
    identifier.AddInstantaneousValue(InstantenousMeasurement(QDateTime(QDate(2017, 1, 1)), 0.0, 1.0));
    QVERIFY(xray);
    QCOMPARE(det_peak, real_peak);
    xray = false;
}

}

