#include <QtTest/QtTest>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class TestMeasurement : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestEgality();
    void TestCanContainData();
    void TestMinMaxValues();
    void TestSetHex();
    void TestSetBitField();
    void TestSetRawData();
    void TestSetInterpretedData();
    void TestIncrement();
    void TestDecrement();
    void TestInvertSign();
    void TestOneBit();

private:
};

}
