include(../../../MedicycCS2.pri)

TEMPLATE = lib
TARGET = MultiplexedNSingleSimulator
DEFINES = MULTIPLEXEDNSINGLESIMULATOR_LIB

DBUS_ADAPTORS += $$PWD/../../../Hardware/NSingle/src/NSingleController.xml
DBUS_ADAPTORS += $$PWD/../../../Hardware/NSingle/src/NSingle.xml
DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml
                 
QT += gui sql network widgets dbus

HEADERS +=  MultiplexedNSingleSimulator.h

SOURCES += MultiplexedNSingleSimulator.cpp

FORMS += MultiplexedNSingleSimulator.ui

unix: RESOURCES += ../../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/NSingle/src -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src
DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src

RESOURCES += resources.qrc

   
   
