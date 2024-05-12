#include <QtTest>
#include "TestFINS.h"
#include "TestMemoryAreaCommand.h"
#include "TestOmronRepo.h"

namespace omron = medicyc::cyclotroncontrolsystem::hardware::omron;

int main(int argc, char** argv) {
//    QApplication app(argc, argv);

    omron::TestFINS testFINS;
    omron::TestMemoryAreaCommand testMemoryAreaCommand;
    omron::TestOmronRepo testOmronRepo;
    return QTest::qExec(&testFINS, argc, argv) |
            QTest::qExec(&testMemoryAreaCommand, argc, argv)  |
            QTest::qExec(&testOmronRepo, argc, argv);
}

