#include "TestMemoryAreaCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

void TestMemoryAreaCommand::initTestCase() {}

void TestMemoryAreaCommand::cleanupTestCase() {}

void TestMemoryAreaCommand::init() {}

void TestMemoryAreaCommand::cleanup() {}

FINSHeader TestMemoryAreaCommand::GetExampleFINSHeader() const {
    return FINSHeader(33, 241);
}

void TestMemoryAreaCommand::TestMemoryAreaReadWord() {
    MemoryAreaRead m1(GetExampleFINSHeader(), FINS_MEMORY_AREA::DM, 4);
    QByteArray expected1;
    expected1.append((char)0x46); // F
    expected1.append((char)0x49); // I
    expected1.append((char)0x4E); // N
    expected1.append((char)0x53); // S
    expected1.append((char)0x00); // Length byte 1
    expected1.append((char)0x00); // Length byte 2
    expected1.append((char)0x00); // Length byte 3
    expected1.append((char)0x1A); // Length byte 4
    expected1.append((char)0x00); // Command code byte 1
    expected1.append((char)0x00); // Command code byte 2
    expected1.append((char)0x00); // Command code byte 3
    expected1.append((char)0x02); // Command code byte 4
    expected1.append((char)0x00); // Error code byte 1
    expected1.append((char)0x00); // Error code byte 2
    expected1.append((char)0x00); // Error code byte 3
    expected1.append((char)0x00); // Error code byte 4
    expected1.append((char)0x80); // ICF
    expected1.append((char)0x00); // RSV
    expected1.append((char)0x02); // GCT
    expected1.append((char)0x00); // DNA
    expected1.append((char)0x21); // DA1
    expected1.append((char)0x00); // DA2
    expected1.append((char)0x00); // SNA
    expected1.append((char)0xF1); // SA1
    expected1.append((char)0x00); // SA2
    expected1.append((char)0x00); // SID
    expected1.append((char)0x01); // MRC
    expected1.append((char)0x01); // SRC
    expected1.append((char)0x82); // Memory Area Code
    expected1.append((char)0x00); // Address 1
    expected1.append((char)0x04); // Address 2
    expected1.append((char)0x00); // Bit
    expected1.append((char)0x00); // No of items 1
    expected1.append((char)0x01); // No of items 2
    QCOMPARE(m1.Command(), expected1);

    MemoryAreaRead m2(GetExampleFINSHeader(), FINS_MEMORY_AREA::CIO, 65535);
    QByteArray expected2;
    expected2.append((char)0x46); // F
    expected2.append((char)0x49); // I
    expected2.append((char)0x4E); // N
    expected2.append((char)0x53); // S
    expected2.append((char)0x00); // Length byte 1
    expected2.append((char)0x00); // Length byte 2
    expected2.append((char)0x00); // Length byte 3
    expected2.append((char)0x1A); // Length byte 4
    expected2.append((char)0x00); // Command code byte 1
    expected2.append((char)0x00); // Command code byte 2
    expected2.append((char)0x00); // Command code byte 3
    expected2.append((char)0x02); // Command code byte 4
    expected2.append((char)0x00); // Error code byte 1
    expected2.append((char)0x00); // Error code byte 2
    expected2.append((char)0x00); // Error code byte 3
    expected2.append((char)0x00); // Error code byte 4
    expected2.append((char)0x80); // ICF
    expected2.append((char)0x00); // RSV
    expected2.append((char)0x02); // GCT
    expected2.append((char)0x00); // DNA
    expected2.append((char)0x21); // DA1
    expected2.append((char)0x00); // DA2
    expected2.append((char)0x00); // SNA
    expected2.append((char)0xF1); // SA1
    expected2.append((char)0x00); // SA2
    expected2.append((char)0x00); // SID
    expected2.append((char)0x01); // MRC
    expected2.append((char)0x01); // SRC
    expected2.append((char)0xB0); // Memory Area Code
    expected2.append((char)0xFF); // Address 1
    expected2.append((char)0xFF); // Address 2
    expected2.append((char)0x00); // Bit
    expected2.append((char)0x00); // No of items 1
    expected2.append((char)0x01); // No of items 2
    QCOMPARE(m2.Command(), expected2);
}

void TestMemoryAreaCommand::TestMemoryAreaWriteWord() {
    MemoryAreaWrite m1(GetExampleFINSHeader(), FINS_MEMORY_AREA::DM, 7, 0x2F1D);
    QByteArray expected1;
    expected1.append((char)0x46); // F
    expected1.append((char)0x49); // I
    expected1.append((char)0x4E); // N
    expected1.append((char)0x53); // S
    expected1.append((char)0x00); // Length byte 1
    expected1.append((char)0x00); // Length byte 2
    expected1.append((char)0x00); // Length byte 3
    expected1.append((char)0x1C); // Length byte 4
    expected1.append((char)0x00); // Command code byte 1
    expected1.append((char)0x00); // Command code byte 2
    expected1.append((char)0x00); // Command code byte 3
    expected1.append((char)0x02); // Command code byte 4
    expected1.append((char)0x00); // Error code byte 1
    expected1.append((char)0x00); // Error code byte 2
    expected1.append((char)0x00); // Error code byte 3
    expected1.append((char)0x00); // Error code byte 4
    expected1.append((char)0x80); // ICF
    expected1.append((char)0x00); // RSV
    expected1.append((char)0x02); // GCT
    expected1.append((char)0x00); // DNA
    expected1.append((char)0x21); // DA1
    expected1.append((char)0x00); // DA2
    expected1.append((char)0x00); // SNA
    expected1.append((char)0xF1); // SA1
    expected1.append((char)0x00); // SA2
    expected1.append((char)0x00); // SID
    expected1.append((char)0x01); // MRC
    expected1.append((char)0x02); // SRC
    expected1.append((char)0x82); // Memory Area Code
    expected1.append((char)0x00); // Address 1
    expected1.append((char)0x07); // Address 2
    expected1.append((char)0x00); // Bit
    expected1.append((char)0x00); // No of items 1
    expected1.append((char)0x01); // No of items 2
    expected1.append((char)0x2F); // Content MSB
    expected1.append((char)0x1D); // Content LSB
    QCOMPARE(m1.Command(), expected1);
}


}
