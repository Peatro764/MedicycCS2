include(../../../MedicycCS2.pri)

QT += sql network testlib widgets

CONFIG += qt warn_on depend_includepath testcase

LIBS += -L/usr/local/lib

TEMPLATE = app

TARGET = TestIOR

HEADERS += TestUtils.h TestIORRepo.h

SOURCES += TestUtils.cpp TestIORRepo.cpp testmain.cpp

RESOURCES = resources.qrc

LIBS += \
    -L$$OUT_PWD/../src -lIOR \
    -L$$OUT_PWD/../../IOR/src -lIOR \
    -L$$OUT_PWD/../../../Global/src -lGlobal \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$OUT_PWD/../src \
    $$PWD/../src \
    $$PWD/../../IOR/src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../src \
    $$PWD/../../IOR/src \
    $$PWD/../../../Global/src \
    $$PWD/../../ClientConnection/src
