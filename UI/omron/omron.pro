include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = OmronGui

DBUS_INTERFACES += $$PWD/../../MessageLogger/src/MessageLogger.xml

QT += gui sql network widgets

HEADERS +=  OmronGui.h

SOURCES += OmronGui.cpp main.cpp

FORMS += OmronGui.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$OUT_PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
