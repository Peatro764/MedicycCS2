#include "TestFINS.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

void TestFINS::initTestCase() {}

void TestFINS::cleanupTestCase() {}

void TestFINS::init() {}

void TestFINS::cleanup() {}

void TestFINS::TestFINSHeader() {
    QByteArray ba1;
    ba1.resize(10);
    ba1[0] = (char)1;
    ba1[1] = (char)2;
    ba1[2] = (char)3;
    ba1[3] = (char)4;
    ba1[4] = (char)5;
    ba1[5] = (char)6;
    ba1[6] = (char)7;
    ba1[7] = (char)8;
    ba1[8] = (char)9;
    ba1[9] = (char)10;

    FINSHeader header1(ba1);
    QCOMPARE(header1.toByteArray(), ba1);

    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::ICF), static_cast<uint8_t>(1));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::RSV), static_cast<uint8_t>(2));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::GCT), static_cast<uint8_t>(3));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DNA), static_cast<uint8_t>(4));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1), static_cast<uint8_t>(5));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DA2), static_cast<uint8_t>(6));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SNA), static_cast<uint8_t>(7));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1), static_cast<uint8_t>(8));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA2), static_cast<uint8_t>(9));
    QCOMPARE(header1.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SID), static_cast<uint8_t>(10));

    FINSHeader header2(1, 2);
    QCOMPARE(header2.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1), static_cast<uint8_t>(1));
    QCOMPARE(header2.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1), static_cast<uint8_t>(2));

    header2.SetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1, 9);
    QCOMPARE(header2.GetFrameField(FINSHeader::FINS_HEADER_FIELD::DA1), static_cast<uint8_t>(9));
    header2.SetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1, 8);
    QCOMPARE(header2.GetFrameField(FINSHeader::FINS_HEADER_FIELD::SA1), static_cast<uint8_t>(8));
}

void TestFINS::TestFINSCommandData() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = (char)1;
    ba1[1] = (char)2;
    ba1.append(QByteArray("FFFF"));

    FINSCommandData cmd_data1(ba1);
    QCOMPARE(cmd_data1.MRC(), static_cast<uint8_t>(1));
    QCOMPARE(cmd_data1.SRC(), static_cast<uint8_t>(2));
    QCOMPARE(cmd_data1.body(), QByteArray("FFFF"));

    FINSCommandData cmd_data2(1, 2, QByteArray("FFFF"));
    QCOMPARE(cmd_data2.MRC(), static_cast<uint8_t>(1));
    QCOMPARE(cmd_data2.SRC(), static_cast<uint8_t>(2));
    QCOMPARE(cmd_data2.body(), QByteArray("FFFF"));
}

void TestFINS::TestFINSFrame() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = (char)1;
    ba1[1] = (char)2;
    ba1.append(QByteArray("FFFF"));
    FINSCommandData cmd_data1(ba1);

    FINSHeader header1(1, 2);

    FINSFrame frame(header1, cmd_data1);
    QByteArray ba_frame = header1.toByteArray();
    ba_frame.append(cmd_data1.toByteArray());
    QCOMPARE(frame.toByteArray(), ba_frame);
}

void TestFINS::TestIsValidFINSReply() {
    QByteArray ba1("FINS123456789123");
    QCOMPARE(IsValidFINSReply(ba1), true);

    QByteArray ba2("FFNS123456789123");
    QCOMPARE(IsValidFINSReply(ba2), false);

    QByteArray ba3("FINS23456789123");
    QCOMPARE(IsValidFINSReply(ba3), false);
}

void TestFINS::TestParseFINSErrorCode() {
    QByteArray ba1("FINS123456780000");
    ba1[12] = (char)0;
    ba1[13] = (char)0;
    ba1[14] = (char)0;
    ba1[15] = (char)3;
    QCOMPARE(static_cast<int>(ParseFINSErrorCode(ba1)), static_cast<int>(3));

    try {
         ParseFINSErrorCode(QByteArray("11111111"));
         QFAIL("TestFINS::TestParseFINSErrorCode Test should have thrown, but didnt");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestParseFINSReplyType() {
    QByteArray ba1("FINS000000000000");
    ba1[8] = (char)0;
    ba1[9] = (char)0;
    ba1[10] = (char)0;
    ba1[11] = (char)3;
    QCOMPARE(ParseFINSReplyType(ba1), FINS_COMMAND_TYPE::FRAME_ERROR);

    try {
         ParseFINSReplyType(QByteArray("4545454545"));
         QFAIL("TestFINS::TestParseFINSReplyType Test should have thrown, but didnt");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestAsByteArray() {
    QByteArray ba1 = AsByteArray(255, 1);
    QCOMPARE(ba1.size(), 1);
    QCOMPARE(ba1.at(0), (char)0xFF);

    QByteArray ba2 = AsByteArray(255, 2);
    QCOMPARE(ba2.size(), 2);
    QCOMPARE(ba2.at(0), (char)0x0);
    QCOMPARE(ba2.at(1), (char)0xFF);

    QByteArray ba3 = AsByteArray(256, 2);
    QCOMPARE(ba3.size(), 2);
    QCOMPARE(ba3.at(0), (char)0x1);
    QCOMPARE(ba3.at(1), (char)0x0);

    QByteArray ba4 = AsByteArray(256, 3);
    QCOMPARE(ba4.size(), 3);
    QCOMPARE(ba4.at(0), (char)0x0);
    QCOMPARE(ba4.at(1), (char)0x1);
    QCOMPARE(ba4.at(2), (char)0x0);

    QByteArray ba5 = AsByteArray(256 + 255, 2);
    QCOMPARE(ba5.size(), 2);
    QCOMPARE(ba5.at(0), (char)0x1);
    QCOMPARE(ba5.at(1), (char)0xFF);

    try {
        AsByteArray(256, 1);
        QFAIL("TestAsByteArray Test should have failed");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestAsInt32() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = (char)0x1;
    ba1[1] = (char)0xFF;
    QCOMPARE(AsInt32(ba1), (uint32_t)(256 + 255));

    QByteArray ba2;
    ba2.resize(2);
    ba2[0] = (char)0x0;
    ba2[1] = (char)0x01;
    QCOMPARE(AsInt32(ba2), (uint32_t)(1));

    QByteArray ba3;
    ba3.resize(3);
    ba3[0] = (char)0x0;
    ba3[1] = (char)0x0;
    ba3[2] = (char)0x0;
    QCOMPARE(AsInt32(ba3), (uint32_t)(0));

    try {
        QByteArray ba4("11111");
        AsInt32(ba4);
        QFAIL("TestFINS::TestAsInt Test should have thrown, but didn't");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestAsInt16() {
    QByteArray ba1;
    ba1.resize(2);
    ba1[0] = (char)0x1;
    ba1[1] = (char)0xFF;
    QCOMPARE(AsInt16(ba1), (uint16_t)(256 + 255));

    QByteArray ba2;
    ba2.resize(1);
    ba2[0] = (char)0xA;
    QCOMPARE(AsInt16(ba2), (uint16_t)(10));

    try {
        QByteArray ba4("111");
        AsInt16(ba4);
        QFAIL("TestFINS::TestAsInt16 Test should have thrown, but didn't");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestAsInt8() {
    QByteArray ba1;
    ba1.resize(1);
    ba1[0] = (char)0x1A;
    QCOMPARE(AsInt8(ba1), (uint8_t)(16 + 10));

    try {
        QByteArray ba2("11");
        AsInt8(ba2);
        QFAIL("TestFINS::TestAsInt8 Test should have thrown, but didn't");
    }
    catch(std::exception& exc) {}
}

void TestFINS::TestHashedAddress() {
    QString hash1 = HashedAddress(1, FINS_MEMORY_AREA::DM, 247, 15);
    QCOMPARE(hash1, QString("1DM24715"));

    QString hash2 = HashedAddress(3, FINS_MEMORY_AREA::UNKNOWN, 0, 3);
    QCOMPARE(hash2, QString("3Unknown03"));
}

}
