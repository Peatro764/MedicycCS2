include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = Source

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += widgets sql network dbus

HEADERS +=  \
Source.h \

SOURCES += \
Source.cpp

DBUS_INTERFACES += \
    $$PWD/../../Hardware/SourcePowerSupply/src/SourcePowerSupply.xml \
    $$PWD/../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/Source.xml

RESOURCES += Source.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../Utils/ -lUtils \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/ \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/SourcePowerSupply/src/ \
    $$PWD/../../Hardware/ClientConnection/src/

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/ \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/SourcePowerSupply/src/ \
    $$PWD/../../Hardware/ClientConnection/src/
