include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = CoolingSystem

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += widgets sql network dbus

HEADERS += CoolingSystem.h

SOURCES += CoolingSystem.cpp

DBUS_INTERFACES += \
    $$PWD/../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/CoolingSystem.xml

RESOURCES += CoolingSystem.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../Utils/ -lUtils

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/
