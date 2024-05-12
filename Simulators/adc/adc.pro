include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = ADCSim

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += $$PWD/../../Hardware/ADC/src/ADC.xml

QT += sql widgets network dbus

HEADERS += ADCSim.h VirtualADC.h

SOURCES += ADCSim.cpp VirtualADC.cpp main.cpp

FORMS += ADCSim.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/ADC/src/ -lADC \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$OUT_PWD/../../Hardware/ADC/src \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ADC/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ADC/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
