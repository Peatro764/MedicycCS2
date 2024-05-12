include(../../../MedicycCS2.pri)

QT +=  gui sql network testlib widgets

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestUtil

HEADERS += TestUtil.h

SOURCES += TestUtil.cpp testmain.cpp

LIBS += -L$$OUT_PWD/../src/ -lClientConnection

INCLUDEPATH += $$PWD/../src

DEPENDPATH += $$PWD/../src
