#include <QtTest/QtTest>

#include "FINS.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class TestFINS : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void TestFINSHeader();
    void TestFINSCommandData();
    void TestFINSFrame();

    void TestIsValidFINSReply();
    void TestParseFINSErrorCode();
    void TestParseFINSReplyType();

    void TestAsByteArray();
    void TestAsInt32();
    void TestAsInt16();
    void TestAsInt8();

    void TestHashedAddress();


private:
};

}
