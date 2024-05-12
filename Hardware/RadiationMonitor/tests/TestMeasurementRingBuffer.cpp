#include "TestMeasurementRingBuffer.h"

#include <iostream>
#include <QDateTime>

#include "MeasurementRingBuffer.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

void TestMeasurementRingBuffer::initTestCase() {
}

void TestMeasurementRingBuffer::cleanupTestCase() {}

void TestMeasurementRingBuffer::init() {}

void TestMeasurementRingBuffer::cleanup() {}

void TestMeasurementRingBuffer::AddDataToRingBuffer() {
    MeasurementRingBuffer buffer(3);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 4)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
}

void TestMeasurementRingBuffer::AddOlderDataToRingBuffer() {
    MeasurementRingBuffer buffer(3);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
}


void TestMeasurementRingBuffer::GetIntegratedCharge_NoBufferOverflow() {
    MeasurementRingBuffer buffer(10);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));

    const double charge_entire_buffer = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 9)));
    QCOMPARE(charge_entire_buffer, 22.0000);

    const double charge_partial_buffer_1 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 8)));
    QCOMPARE(charge_partial_buffer_1, 15.0000);

    const double charge_partial_buffer_2 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 8)));
    QCOMPARE(charge_partial_buffer_2, 15.0000);

    const double charge_partial_buffer_3 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 4)));
    QCOMPARE(charge_partial_buffer_3, 0.0000);

    const double charge_partial_buffer_4 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 5)));
    QCOMPARE(charge_partial_buffer_4, 10.0000);

    const double charge_partial_buffer_5 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 6)));
    QCOMPARE(charge_partial_buffer_5, 10.0000);

    const double charge_partial_buffer_6 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 4)));
    QCOMPARE(charge_partial_buffer_6, 0.0000);

    // datetime with second precision
    MeasurementRingBuffer buffer2(10);
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 33)), 10.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 35)), 20.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 37)), 25.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 39)), 32.0, 0.0));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 3, 30)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 3, 30)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 3), QTime(3, 3, 30)), 10.0, 0.0)));

    const double charge_entire_buffer_2 = buffer2.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 33)),
                                                                      QDateTime(QDate(2017, 3, 3), QTime(3, 4, 39)));
    QCOMPARE(charge_entire_buffer_2, 22.0000);

    const double charge_partial_buffer_7 = buffer2.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3), QTime(3, 4, 35)),
                                                                       QDateTime(QDate(2017, 3, 3), QTime(3, 4, 38)));
    QCOMPARE(charge_partial_buffer_7, 5.0000);
}

void TestMeasurementRingBuffer::GetIntegratedCharge_SingleBufferOverflow() {
    // Buffer overflow in the middle
    MeasurementRingBuffer buffer(10);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));

    const double charge_entire_buffer = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 9)));
    QCOMPARE(charge_entire_buffer, 32.0000);

    const double charge_partial_buffer_1 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 5)));
    QCOMPARE(charge_partial_buffer_1, 10.0000);

    // Buffer overflow at first or last event
    MeasurementRingBuffer buffer2(10);
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer2.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 10.0, 0.0)));
    buffer2.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 10.0, 0.0)));

    const double charge_partial_buffer_2 = buffer2.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 5)));
    QCOMPARE(charge_partial_buffer_2, 20.0000);

    const double charge_partial_buffer_3 = buffer2.GetIntegratedCharge(QDateTime(QDate(2017, 3, 5)), QDateTime(QDate(2017, 3, 7)));
    QCOMPARE(charge_partial_buffer_3, 5.0000);

    // Buffer overflow in the middle, second integrated value smaller than the first one
    MeasurementRingBuffer buffer3(10);
    buffer3.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer3.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer3.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 2.0, 0.0));
    buffer3.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 8.0, 0.0));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 23.0, 0.0)));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 23.0, 0.0)));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 23.0, 0.0)));
    buffer3.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 23.0, 0.0)));

    const double charge_entire_buffer_3 = buffer3.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 9)));
    QCOMPARE(charge_entire_buffer_3, 21.000);
}




void TestMeasurementRingBuffer::GetIntegratedCharge_MultipleBufferOverflow() {
    // Buffer overflow in the middle
    MeasurementRingBuffer buffer(10);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 11)), 42.0, 0.0));

    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 2)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 6)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 8)), 10.0, 0.0)));
    buffer.AddBufferIntegratedValue(IntegratedMeasurementBuffer(IntegratedMeasurement(), IntegratedMeasurement(QDateTime(QDate(2017, 3, 8)), 10.0, 0.0)));

    const double charge_entire_buffer = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 11)));
    QCOMPARE(charge_entire_buffer, 52.0000);

    const double charge_partial_buffer_1 = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 4)), QDateTime(QDate(2017, 3, 7)));
    QCOMPARE(charge_partial_buffer_1, 25.0000 );
}

void TestMeasurementRingBuffer::GetIntegratedCharge_NoDataAvailable() {
    MeasurementRingBuffer buffer(10);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 11)), 42.0, 0.0));

    try {
        buffer.GetIntegratedCharge(QDateTime(QDate(2017, 2, 3)), QDateTime(QDate(2017, 3, 11)));
        QFAIL("GetIntegratedCharge_MultipleBufferOverflow Should have thrown");
    }
    catch (std::exception& exc) {}
}

void TestMeasurementRingBuffer::GetIntegratedCharge_NoBufferData() {
    MeasurementRingBuffer buffer(10);
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 3)), 10.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 5)), 20.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 7)), 25.0, 0.0));
    buffer.AddIntegratedValue(IntegratedMeasurement(QDateTime(QDate(2017, 3, 9)), 32.0, 0.0));

    const double charge_entire_buffer = buffer.GetIntegratedCharge(QDateTime(QDate(2017, 3, 3)), QDateTime(QDate(2017, 3, 9)));
    QCOMPARE(charge_entire_buffer, 22.0000);
}

}
