include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = VacuumSystem

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += widgets sql network dbus

HEADERS += VacuumSystem.h

SOURCES += VacuumSystem.cpp

DBUS_INTERFACES += \
    $$PWD/../../Hardware/IOR/src/IOR.xml \
    $$PWD/../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/VacuumSystem.xml

RESOURCES += VacuumSystem.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../Utils/ -lUtils

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/
