include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = BeamLineState

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += \
    $$PWD/../../../MiddleLayer/BeamLineState/BeamLineState.xml

RESOURCES += \
    ../../../MiddleLayer/BeamLineState/BeamLineState.xml

DBUS_INTERFACES += \
    $$PWD/../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../../Hardware/IOR/src/IOR.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../MiddleLayer/Utils/ -lUtils \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../MiddleLayer/BeamLineState/ -lBeamLineState

INCLUDEPATH += \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/BeamLineState

DEPENDPATH += \
    $$PWD/../../../MiddleLayer/Utils \
    $$PWD/../../../Global/src \
    $$PWD/../../../MiddleLayer/BeamLineState
