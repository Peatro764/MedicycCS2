#include "TestNodeAddressCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

void TestNodeAddressCommand::initTestCase() {}

void TestNodeAddressCommand::cleanupTestCase() {}

void TestNodeAddressCommand::init() {}

void TestNodeAddressCommand::cleanup() {}

void TestNodeAddressCommand::TestNodeAddressRequest() {
    NodeAddressRequest r1;
    QByteArray expected1;
    expected1.append((char)0x46); // F
    expected1.append((char)0x49); // I
    expected1.append((char)0x4E); // N
    expected1.append((char)0x53); // S
    expected1.append((char)0x00); // Length byte 1
    expected1.append((char)0x00); // Length byte 2
    expected1.append((char)0x00); // Length byte 3
    expected1.append((char)0x0C); // Length byte 4
    expected1.append((char)0x00); // Command code byte 1
    expected1.append((char)0x00); // Command code byte 2
    expected1.append((char)0x00); // Command code byte 3
    expected1.append((char)0x00); // Command code byte 4
    expected1.append((char)0x00); // Error code byte 1
    expected1.append((char)0x00); // Error code byte 2
    expected1.append((char)0x00); // Error code byte 3
    expected1.append((char)0x00); // Error code byte 4
    expected1.append((char)0x00); // Client node address 1
    expected1.append((char)0x00); // Client node address 2
    expected1.append((char)0x00); // Client node address 3
    expected1.append((char)0x00); // Client node address 4
    QCOMPARE(r1.Command(), expected1);
}

void TestNodeAddressCommand::TestNodeAddressReply() {
    QByteArray expected1;
    expected1.append((char)0x46); // F
    expected1.append((char)0x49); // I
    expected1.append((char)0x4E); // N
    expected1.append((char)0x53); // S
    expected1.append((char)0x00); // Length byte 1
    expected1.append((char)0x00); // Length byte 2
    expected1.append((char)0x00); // Length byte 3
    expected1.append((char)0x0C); // Length byte 4
    expected1.append((char)0x00); // Command code byte 1
    expected1.append((char)0x00); // Command code byte 2
    expected1.append((char)0x00); // Command code byte 3
    expected1.append((char)0x00); // Command code byte 4
    expected1.append((char)0x00); // Error code byte 1
    expected1.append((char)0x00); // Error code byte 2
    expected1.append((char)0x00); // Error code byte 3
    expected1.append((char)0x00); // Error code byte 4
    expected1.append((char)0x00); // Client node address 1
    expected1.append((char)0x00); // Client node address 2
    expected1.append((char)0x00); // Client node address 3
    expected1.append((char)0xFE); // Client node address 4
    expected1.append((char)0x00); // Server node address 1
    expected1.append((char)0x00); // Server node address 2
    expected1.append((char)0x00); // Server node address 3
    expected1.append((char)0x13); // Server node address 4

    NodeAddressReply r1(expected1);
    QCOMPARE(r1.ClientNodeAddress(), 0xFE);
    QCOMPARE(r1.ServerNodeAddress(), 0x13);
}


}
