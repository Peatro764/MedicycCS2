include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = IORSim

DBUS_ADAPTORS += $$PWD/../../Hardware/IOR/src/IOR.xml

QT += gui network sql widgets dbus

HEADERS +=  IORSim.h VirtualIOR.h

SOURCES += IORSim.cpp VirtualIOR.cpp main.cpp

FORMS += IORSim.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/IOR/src/ -lIOR \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$OUT_PWD/../../Hardware/IOR/src \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
