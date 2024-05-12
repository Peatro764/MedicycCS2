include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = SignalGeneratorGui

QT += gui sql network widgets

HEADERS +=  SignalGeneratorGui.h

SOURCES += SignalGeneratorGui.cpp main.cpp

FORMS += SignalGeneratorGui.ui

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../Hardware/SignalGenerator/src/ -lSignalGenerator \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/SignalGenerator/src \
    $$OUT_PWD/../../Hardware/SignalGenerator/src \
    $$PWD/../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../Hardware/SignalGenerator/src \
    $$PWD/../../Hardware/ClientConnection/src

RESOURCES += resources.qrc
