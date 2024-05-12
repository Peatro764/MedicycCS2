include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = IORGui

DBUS_INTERFACES += $$PWD/../../MessageLogger/src/MessageLogger.xml

QT += gui network sql widgets dbus

HEADERS +=  IORGui.h

SOURCES += IORGui.cpp main.cpp

FORMS += IORGui.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/IOR/src/ -lIOR \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
