#include "TestReply.h"
#include "Reply.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestReply::initTestCase() {}

void TestReply::cleanupTestCase() {}

void TestReply::init() {}

void TestReply::cleanup() {}

void TestReply::TestIsValid() {
    QCOMPARE(Reply("$A=F02E*\n\r").IsValid(), true);
    QCOMPARE(Reply("$A=F02D*\n\r").IsValid(), false);
    QCOMPARE(Reply("$A=F02E*").IsValid(), false);
    QCOMPARE(Reply("$A?F02E*\n\r").IsValid(), false);
}

void TestReply::TestHeader() {
    QCOMPARE(Reply("$A=XXSS*\n\r").Header(), QString("$A="));
    QCOMPARE(Reply("$A=XXSS").Header(), QString(""));
}


void TestReply::TestType() {
    QCOMPARE(Reply("$A=XXSS*\n\r").Type(), ReplyType::A);
    QCOMPARE(Reply("$A=XXSS").Type(), ReplyType::UNKNOWN);
}

void TestReply::TestBody() {
    QCOMPARE(Reply("$A=XXSS*\n\r").Body(), QString("XX"));
    QCOMPARE(Reply("$A=XXXXSS*\n\r").Body(), QString("XXXX"));
    QCOMPARE(Reply("$A=XXSS").Header(), QString(""));
}

void TestReply::TestFooter() {
    QCOMPARE(Reply("$A=XXSS*\n\r").Footer(), QString("*\n\r"));
    QCOMPARE(Reply("$A=XXXXSS*\n\r").Footer(), QString("*\n\r"));
    QCOMPARE(Reply("$A=XXSS").Footer(), QString(""));
}

void TestReply::TestCheckSum() {
    QCOMPARE(Reply("$A=XXSS*\n\r").CheckSum(), QString("SS"));
    QCOMPARE(Reply("$A=XXXXSS*\n\r").CheckSum(), QString("SS"));
    QCOMPARE(Reply("$A=XXSS").CheckSum(), QString(""));
}

}
