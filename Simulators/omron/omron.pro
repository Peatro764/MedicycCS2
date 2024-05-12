include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = OmronSim

DBUS_ADAPTORS += $$PWD/../../Hardware/Omron/src/Omron.xml
DBUS_INTERFACES += $$PWD/../../MessageLogger/src/MessageLogger.xml

QT += gui sql network widgets dbus

HEADERS +=  VirtualOmron.h OmronSim.h

SOURCES += VirtualOmron.cpp OmronSim.cpp main.cpp

FORMS += OmronSim.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
