#include <QtTest/QtTest>

#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestRegulatedParameter : public QObject {
    Q_OBJECT
private slots:
    NSingleConfig GetNSingleConfig(int setpoint_lsb, int setpoint_msb, int value_lsb, int value_msb, int signal_variation, int buffer_size);

    bool VerifyCount(QSignalSpy& spy, int expected_count, int timeout);

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void SignalCharacteristics();
    void NearTarget();
    void NearZero();
    void SignalSteady();
    void PolarityCorrect();
    void OnTarget_VaryMean();
    void OnTarget_VaryStdDev();
    void SignalIsEmittedWhenDesiredValueIsUpdated();
    void TestGetImprovedSetPoint();
    void MeasurementContainerDoesNotStartAtBit0();

private:
};

}
