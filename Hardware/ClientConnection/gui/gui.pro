include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = ClientConnectionGui

QT += gui network serialport widgets

HEADERS +=  ClientConnectionGui.h

SOURCES += ClientConnectionGui.cpp main.cpp

FORMS += ClientConnectionGui.ui

unix: LIBS += -L$$OUT_PWD/../src/ -lClientConnection

INCLUDEPATH += $$PWD/../src

DEPENDPATH += $$PWD/../src
