include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = ElectrometerGui

DBUS_INTERFACES += \
    $$PWD/../../Hardware/Electrometer/src/Electrometer.xml

QT += gui sql network widgets charts dbus

HEADERS +=  ElectrometerGui.h SlidingGraph.h

SOURCES += ElectrometerGui.cpp SlidingGraph.cpp main.cpp

FORMS += ElectrometerGui.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection


INCLUDEPATH += \
    $$PWD/../../Hardware/Electrometer/src \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Hardware/Electrometer/src \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc \
              TODO.md
    
