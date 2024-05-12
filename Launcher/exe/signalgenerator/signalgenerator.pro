include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = SignalGenerator

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += ../../../Hardware/SignalGenerator/src/SignalGenerator.xml
DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml

QT += sql widgets network dbus

HEADERS +=

SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Hardware/SignalGenerator/src/ -lSignalGenerator

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../Hardware/SignalGenerator/src

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../Hardware/SignalGenerator/src
