include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = ControllerGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

DBUS_INTERFACES += \
    $$PWD/../../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../../Hardware/SignalGenerator/src/SignalGenerator.xml \
    $$PWD/../../../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml

HEADERS += ControllerGui.h

SOURCES += ControllerGui.cpp main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../src/ -lHF

INCLUDEPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../src/

DEPENDPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../src/

FORMS = ControllerGui.ui
