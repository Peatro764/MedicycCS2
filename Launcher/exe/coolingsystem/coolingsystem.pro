include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = CoolingSystem

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../MiddleLayer/CoolingSystem/CoolingSystem.xml

RESOURCES += \
    ../../../MiddleLayer/CoolingSystem/CoolingSystem.xml

DBUS_INTERFACES += \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../Hardware/ADC/src/ADC.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../MiddleLayer/Utils/ -lUtils \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../MiddleLayer/CoolingSystem/ -lCoolingSystem

INCLUDEPATH += \
    $$PWD/../../../Hardware/Omron/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/CoolingSystem/

DEPENDPATH += \
    $$PWD/../../../Hardware/Omron/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/CoolingSystem/
