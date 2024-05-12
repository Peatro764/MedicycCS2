#include <QtTest>
#include "TestIORRepo.h"
#include "TestUtils.h"

namespace ior = medicyc::cyclotroncontrolsystem::hardware::ior;

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    ior::TestIORRepo testRepo;
    ior::TestUtils testUtils;

    return QTest::qExec(&testRepo, argc, argv) |
           QTest::qExec(&testUtils, argc, argv);
}
