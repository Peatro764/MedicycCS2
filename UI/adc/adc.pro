include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = ADCGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets

HEADERS +=  ADCGui.h

SOURCES += ADCGui.cpp main.cpp

FORMS += ADCGui.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/ADC/src/ -lADC \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ADC/src \
    $$OUT_PWD/../../Hardware/ADC/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/ADC/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
