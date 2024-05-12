include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = HFController

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../../MiddleLayer/HF/src/HFController.xml

RESOURCES +=

DBUS_INTERFACES += \
    $$PWD/../../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../../Hardware/SignalGenerator/src/SignalGenerator.xml \
    $$PWD/../../../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml \
    $$PWD/../../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../../Hardware/Omron/src/Omron.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../../MiddleLayer/HF/src -lHF

INCLUDEPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../MiddleLayer/HF/src

DEPENDPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../../../MiddleLayer/HF/src
