include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = StandardNSingle

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += ../../../../Hardware/NSingle/src/NSingle.xml ../../../../Hardware/NSingle/src/NSingleController.xml
DBUS_INTERFACES += $$PWD/../../../../MessageLogger/src/MessageLogger.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/NSingle/src \
    $$PWD/../../../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/NSingle/src \
    $$PWD/../../../../Hardware/ClientConnection/src
