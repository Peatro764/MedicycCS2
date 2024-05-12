#include <QtTest>

#include "TestMeasurement.h"
#include "TestMeasurementRingBuffer.h"
#include "TestXRayIdentifier.h"
#include "TestRadiationMonitorRepo.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::TestMeasurement testMeasurement;
    medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::TestMeasurementRingBuffer testRingBuffer;
    medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::TestXRayIdentifier testXRayIdentifier;
    medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::TestRadiationMonitorRepo testRadiationMonitorRepo;

    return QTest::qExec(&testMeasurement, argc, argv) |
           QTest::qExec(&testRingBuffer, argc, argv) |
           QTest::qExec(&testXRayIdentifier, argc, argv) |
           QTest::qExec(&testRadiationMonitorRepo, argc, argv);
}
