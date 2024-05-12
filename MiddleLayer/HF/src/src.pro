include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = HF

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += widgets sql network dbus

HEADERS += Niveau.h HFUtils.h Dee.h Membrane.h Accord.h VoltageRegulator.h HFController.h HFRepo.h Configuration.h \
    HFState.h

SOURCES += Niveau.cpp Dee.cpp Membrane.cpp Accord.cpp VoltageRegulator.cpp HFController.cpp HFRepo.cpp Configuration.cpp \
    HFState.cpp

DBUS_INTERFACES += \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../Hardware/SignalGenerator/src/SignalGenerator.xml \
    $$PWD/../../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml \
    $$PWD/../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../Hardware/Omron/src/Omron.xml

RESOURCES += HFController.xml \
             resources.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Utils/ -lUtils

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../Utils/

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../Utils/

