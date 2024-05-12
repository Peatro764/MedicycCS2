include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = Source

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../MiddleLayer/Source/Source.xml

RESOURCES +=
    ../../../MiddleLayer/Source/Source.xml

DBUS_INTERFACES += \
$$PWD/../../../MessageLogger/src/MessageLogger.xml \
$$PWD/../../../Hardware/NSingle/src/NSingleController.xml \
$$PWD/../../../Hardware/SourcePowerSupply/src/SourcePowerSupply.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../MiddleLayer/Utils/ -lUtils \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../MiddleLayer/Source/ -lSource

INCLUDEPATH += \
    $$PWD/../../../Hardware/SourcePowerSupply/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/Source

DEPENDPATH += \
    $$PWD/../../../Hardware/SourcePowerSupply/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/Source
