include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = StandardNSingleGroup

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

HEADERS += StandardNSingleGroup.h

SOURCES += StandardNSingleGroup.cpp

RESOURCES += ../NSingleGroup.xml

DBUS_INTERFACES += \
    $$PWD/../../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/../NSingleGroup.xml

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Utils -lUtils \
    -L$$OUT_PWD/../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../Utils \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../Utils \
    $$PWD/../../../Global/src
