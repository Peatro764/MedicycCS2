include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = UtilsGui

QT += gui network sql widgets

HEADERS +=  UtilsGui.h

SOURCES += UtilsGui.cpp main.cpp

FORMS += UtilsGui.ui

LIBS += \
    -L$$OUT_PWD/../src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../src

DEPENDPATH += \
    $$PWD/../src/

RESOURCES += 
