include(../../MedicycCS2.pri)

QT += testlib sql widgets

CONFIG += qt warn_on depend_includepath testcase

TEMPLATE = app

TARGET = TestCycloConfig

HEADERS += TestCycloConfig.h

SOURCES += TestCycloConfig.cpp testmain.cpp

unix: LIBS += \
    -L$$OUT_PWD/../src/ -lCycloConfig

INCLUDEPATH += \
    $$PWD/../src

DEPENDPATH += \
    $$PWD/../src

RESOURCES = resources.qrc

