#include <QtTest/QtTest>

#include "FINS.h"
#include "MemoryAreaCommand.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class TestMemoryAreaCommand : public QObject {
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    FINSHeader GetExampleFINSHeader() const;

    //void TestMemoryAreaReadBit();
    void TestMemoryAreaReadWord();
    //void TestMemoryAreaWriteBit();
    void TestMemoryAreaWriteWord();

private:
};

}
