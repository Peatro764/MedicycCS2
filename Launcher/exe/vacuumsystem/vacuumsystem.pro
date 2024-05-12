include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = VacuumSystem

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../MiddleLayer/VacuumSystem/VacuumSystem.xml

RESOURCES += \
    ../../../MiddleLayer/VacuumSystem/VacuumSystem.xml

DBUS_INTERFACES += \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../../Hardware/IOR/src/IOR.xml \
    $$PWD/../../../Hardware/ADC/src/ADC.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
-L$$OUT_PWD/../../../Hardware/IOR/src/ -lIOR \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../MiddleLayer/Utils/ -lUtils \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../MiddleLayer/VacuumSystem/ -lVacuumSystem

INCLUDEPATH += \
    $$PWD/../../../Hardware/IOR/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/VacuumSystem/

DEPENDPATH += \
    $$PWD/../../../Hardware/IOR/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/VacuumSystem/
