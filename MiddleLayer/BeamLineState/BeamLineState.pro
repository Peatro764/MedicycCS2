include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = BeamLineState

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += widgets sql network dbus

HEADERS += BeamLineState.h

SOURCES += BeamLineState.cpp

DBUS_INTERFACES += \
    $$PWD/../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../Hardware/IOR/src/IOR.xml

DBUS_ADAPTORS += $$PWD/BeamLineState.xml

RESOURCES += BeamLineState.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../Utils/ -lUtils

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../Utils/
