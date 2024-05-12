include(../MedicycCS2.pri)

QT += core network sql widgets dbus

TARGET = DataLogger
TEMPLATE = app

# DEFINES += QT_NO_DEBUG_OUTPUT
SOURCES += DataLogger.cpp main.cpp
HEADERS += DataLogger.h

DBUS_INTERFACES += \
    $$PWD/../Hardware/ADC/src/ADC.xml \
    $$PWD/../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../Hardware/Electrometer/src/Electrometer.xml \
    $$PWD/../Hardware/RadiationMonitor/src/RadiationMonitor.xml \
    $$PWD/../Hardware/Omron/src/Omron.xml \
    $$PWD/../MessageLogger/src/MessageLogger.xml

unix: LIBS += \
    -L$$OUT_PWD/../Global/src/ -lGlobal \
    -L$$OUT_PWD/../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../Hardware/RadiationMonitor/src/ -lRadiationMonitor \
    -L$$OUT_PWD/../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../Global/src \
    $$PWD/../Hardware/NSingle/src \
    $$PWD/../Hardware/Omron/src \
    $$PWD/../Hardware/RadiationMonitor/src \
    $$PWD/../Hardware/ClientConnection/src
DEPENDPATH += \
    $$PWD/../Global/src \
    $$PWD/../Hardware/NSingle/src \
    $$PWD/../Hardware/Omron/src \
    $$PWD/../Hardware/RadiationMonitor/src \
    $$PWD/../Hardware/ClientConnection/src
