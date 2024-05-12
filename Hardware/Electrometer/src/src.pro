include(../../../MedicycCS2.pri)

QT += network sql dbus

TARGET = Electrometer

TEMPLATE = lib

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += Electrometer_LIB

SOURCES += Electrometer.cpp

HEADERS += IElectrometer.h Electrometer.h Electrometer_global.h

DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml

unix: LIBS += \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

