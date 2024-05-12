include(../../../MedicycCS2.pri)

QT += network testlib sql dbus

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestADC

HEADERS += TestChannel.h TestADCRepo.h

SOURCES += TestChannel.cpp TestADCRepo.cpp testmain.cpp

LIBS += \
    -L$$OUT_PWD/../src/ -lADC \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$OUT_PWD/../src \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src
       
RESOURCES = resources.qrc
