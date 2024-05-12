include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = InjectionNSingleGroup

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += $$PWD/../../../../MiddleLayer/NSingleGroup/NSingleGroup.xml
DBUS_INTERFACES += \
$$PWD/../../../../MessageLogger/src/MessageLogger.xml \
$$PWD/../../../../Hardware/NSingle/src/NSingleController.xml \
$$PWD/../../../../Hardware/NSingle/src/NSingle.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../../MiddleLayer/Utils -lUtils \
    -L$$OUT_PWD/../../../../MiddleLayer/NSingleGroup/standard -lStandardNSingleGroup \
    -L$$OUT_PWD/../../../../MiddleLayer/NSingleGroup/injection/ -lInjectionNSingleGroup

INCLUDEPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/NSingle/src \
    $$PWD/../../../../Hardware/ClientConnection/src \
    $$PWD/../../../../MiddleLayer/Utils \
    $$PWD/../../../../MiddleLayer/NSingleGroup/standard \
    $$PWD/../../../../MiddleLayer/NSingleGroup/injection

DEPENDPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/NSingle/src \
    $$PWD/../../../../Hardware/ClientConnection/src \
    $$PWD/../../../../MiddleLayer/Utils \
    $$PWD/../../../../MiddleLayer/NSingleGroup/standard \
    $$PWD/../../../../MiddleLayer/NSingleGroup/injection
