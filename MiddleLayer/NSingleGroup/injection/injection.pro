include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = InjectionNSingleGroup

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

HEADERS +=  InjectionNSingleGroup.h

SOURCES += InjectionNSingleGroup.cpp

DBUS_INTERFACES += \
    $$PWD/../../../Hardware/NSingle/src/NSingle.xml \
    $$PWD/../../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/../NSingleGroup.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Utils/ -lUtils \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../standard/ -lStandardNSingleGroup

INCLUDEPATH += \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../Global/src \
    $$PWD/../../Utils/ \
    $$PWD/../standard

DEPENDPATH += \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../Global/src \
    $$PWD/../../Utils/src \
    $$PWD/../standard
