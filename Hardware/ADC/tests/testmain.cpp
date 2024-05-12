#include <QtTest>
#include "TestChannel.h"
#include "TestADCRepo.h"

namespace adc = medicyc::cyclotroncontrolsystem::hardware::adc;

int main(int argc, char** argv) {
//    QApplication app(argc, argv);

    adc::TestChannel testChannel;
    adc::TestADCRepo testADCRepo;
    return QTest::qExec(&testChannel, argc, argv) |
            QTest::qExec(&testADCRepo, argc, argv);
}

