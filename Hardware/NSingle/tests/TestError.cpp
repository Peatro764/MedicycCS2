#include "TestError.h"
#include "Error.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestError::initTestCase() {}

void TestError::cleanupTestCase() {}

void TestError::init() {}

void TestError::cleanup() {}

void TestError::TestTypes() {
    QMap<QString, Error::Type> types = Error::types();
    QCOMPARE(types.size(), 8);
}

void TestError::TestTypeToString() {
    QCOMPARE(Error::TypeToString(Error::Type::POLARITY_CHANGE_FORBIDDEN) , QString("POLARITY_CHANGE_FORBIDDEN"));
}

void TestError::TestHexCode() {
    Error error(QString("42"));
    QCOMPARE(error.hexcode(), QString("42"));
}

void TestError::TestBitSet() {
    std::bitset<8> bits(5);
    Error error(bits);
    QCOMPARE(error.bitset(), bits);
    QCOMPARE(error.hexcode(), QString("05"));
}

void TestError::TestActive() {
    Error error1(QString("42"));
    QStringList list1 { "INTERNAL_FRAME", "POLARITY_CHANGE_FORBIDDEN" };
    QCOMPARE(error1.active(), list1);

    Error error2(QString("00"));
    QStringList list2;
    QCOMPARE(error2.active(), list2);

    Error error3(QString("B5"));
    QStringList list3 { "OVER_FLOW", "COMMUNICATION", "I_VALUE_NOT_ZERO", "CMD_ON_ACTIVE", "STATE_CHANGE" };
    qDebug() << "1 " << error3.active();
    QCOMPARE(error3.active(), list3);

    Error error4(QString("C8"));
    QStringList list4 { "I_SETPOINT_NOT_ZERO", "POLARITY_CHANGE_FORBIDDEN", "STATE_CHANGE" };
    QCOMPARE(error4.active(), list4);

}

}
