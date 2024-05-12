include(../../../MedicycCS2.pri)

QT += testlib sql

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestSourcePowerSupply

HEADERS += TestPowerSupplyRepo.h

SOURCES += TestPowerSupplyRepo.cpp testmain.cpp

unix: LIBS += \
    -L$$OUT_PWD/../src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../src \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../src \
    $$PWD/../../../Global/src

RESOURCES = ../resources.qrc
