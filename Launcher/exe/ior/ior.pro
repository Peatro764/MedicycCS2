include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = IOR

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += ../../../Hardware/IOR/src/IOR.xml
DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/IOR/src -lIOR \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/IOR/src \
    $$PWD/../../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/IOR/src \
    $$PWD/../../../Hardware/ClientConnection/src
