#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class TestMeasurement : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void MeasurementOperators();
    void IntegratedMeasurementOperators();
    void IntegratedMeasurementBufferOperators();

private:
};

}

