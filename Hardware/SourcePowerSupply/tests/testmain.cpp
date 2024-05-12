#include <QtTest>
#include "TestPowerSupplyRepo.h"

namespace powersupply = medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply;

int main(int argc, char** argv) {
//    QApplication app(argc, argv);

    powersupply::TestPowerSupplyRepo testRepo;

    return QTest::qExec(&testRepo, argc, argv);
}

