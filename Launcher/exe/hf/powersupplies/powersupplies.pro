include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = HFPowerSupplies

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml

RESOURCES +=
    ../../../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml

DBUS_INTERFACES += \
    $$PWD/../../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../../Hardware/Omron/src/Omron.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../../Hardware/HFPowerSupplies/src -lHFPowerSupplies

INCLUDEPATH += \
    $$PWD/../../../../MiddleLayer/Utils \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/HFPowerSupplies/src

DEPENDPATH += \
    $$PWD/../../../../MiddleLayer/Utils \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../Hardware/HFPowerSupplies/src
