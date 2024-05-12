#include <QtTest>
#include <QtWidgets/QApplication>

#include "TestCycloConfig.h"

using namespace cyclo_config;

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    TestCycloConfig testCycloConfig;
    return QTest::qExec(&testCycloConfig, argc, argv);
}

