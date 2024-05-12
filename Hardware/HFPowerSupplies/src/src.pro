include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = HFPowerSupplies

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

HEADERS +=  HFPowerSupplies.h \
    Device.h
# Membrane.h DeeVoltage.h HF.h HFUtils.h

SOURCES += HFPowerSupplies.cpp \
    Device.cpp
# Membrane.cpp DeeVoltae.cpp HF.cpp HFUtils.cpp

DBUS_INTERFACES += \
    $$PWD/../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml

RESOURCES += HFPowerSupplies.xml

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../../../Global/src
