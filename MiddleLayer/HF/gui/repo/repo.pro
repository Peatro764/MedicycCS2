include(../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = RepoGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

DBUS_INTERFACES += \
    $$PWD/../../../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../../../MessageLogger/src/MessageLogger.xml

HEADERS += RepoGui.h

SOURCES += RepoGui.cpp main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../src/ -lHF

INCLUDEPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../src/

DEPENDPATH += \
    $$PWD/../../../../Global/src \
    $$PWD/../../src
