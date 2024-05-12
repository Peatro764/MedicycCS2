#include <QtTest>
#include "TestQueue.h"
#include "TestGlobalRepo.h"

int main(int argc, char** argv) {
//    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::global::TestQueue testQueue;
    medicyc::cyclotroncontrolsystem::global::TestGlobalRepo testGlobalRepo;
    return QTest::qExec(&testQueue, argc, argv) | QTest::qExec(&testGlobalRepo, argc, argv);
}

