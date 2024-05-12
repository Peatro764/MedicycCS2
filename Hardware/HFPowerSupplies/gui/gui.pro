include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = HFGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

DBUS_INTERFACES += \
    $$PWD/../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml

HEADERS += HFGui.h
# Membrane.h DeeVoltage.h HF.h HFUtils.h

SOURCES += HFGui.cpp main.cpp
# Membrane.cpp DeeVoltage.cpp HF.cpp HFUtils.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../src/ -lHFPowerSupplies

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../src/

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../src

FORMS = HFGui.ui
