include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = SourcePowerSupply

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    ../../../Hardware/SourcePowerSupply/src/SourcePowerSupply.xml

DBUS_INTERFACES += \
$$PWD/../../../MessageLogger/src/MessageLogger.xml \
$$PWD/../../../Hardware/ADC/src/ADC.xml \
$$PWD/../../../Hardware/Omron/src/Omron.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/SourcePowerSupply/src

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/SourcePowerSupply/src
