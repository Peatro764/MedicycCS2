include(../../../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = StandardNSingleSimulator

DBUS_ADAPTORS += ../../../../../Hardware/NSingle/src/NSingle.xml ../../../../../Hardware/NSingle/src/NSingleController.xml
DBUS_INTERFACES += $$PWD/../../../../../MessageLogger/src/MessageLogger.xml

QT += gui sql network widgets dbus

HEADERS += 
SOURCES += main.cpp
FORMS +=

unix: RESOURCES += ../../../../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../../../Hardware/NSingle/src -lNSingle \
    -L$$OUT_PWD/../../../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../../../Simulators/nsingle/standard -lStandardNSingleSimulator

INCLUDEPATH += \
    $$PWD/../../../../../Global/src \
    $$PWD/../../../../../Hardware/NSingle/src \
    $$PWD/../../../../../Hardware/ClientConnection/src \
    $$PWD/../../../../../Simulators/nsingle/standard 

DEPENDPATH += \
    $$PWD/../../../../../Global/src \
    $$PWD/../../../../../Hardware/NSingle/src \
    $$PWD/../../../../../Hardware/ClientConnection/src \
    $$PWD/../../../../../Simulators/nsingle/standard 

