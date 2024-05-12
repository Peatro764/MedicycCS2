include(../../../MedicycCS2.pri)

QT += testlib network sql

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestOmron

HEADERS += TestOmronRepo.h TestFINS.h TestMemoryAreaCommand.h TestNodeAddressCommand.h

SOURCES += TestOmronRepo.cpp TestFINS.cpp TestMemoryAreaCommand.cpp TestNodeAddressCommand.cpp testmain.cpp

unix: LIBS += \
    -L$$OUT_PWD/../src/ -lOmron \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$OUT_PWD/../src \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src/

DEPENDPATH += \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src/

RESOURCES = resources.qrc
