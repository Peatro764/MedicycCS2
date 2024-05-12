#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class TestMeasurementRingBuffer : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void AddDataToRingBuffer();
    void AddOlderDataToRingBuffer();
    void GetIntegratedCharge_NoBufferOverflow();
    void GetIntegratedCharge_SingleBufferOverflow();
    void GetIntegratedCharge_MultipleBufferOverflow();
    void GetIntegratedCharge_NoDataAvailable();
    void GetIntegratedCharge_NoBufferData();

private:
};

}

