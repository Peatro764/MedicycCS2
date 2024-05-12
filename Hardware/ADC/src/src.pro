include(../../../MedicycCS2.pri)

QT += network sql dbus

TARGET = ADC

TEMPLATE = lib

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += ADC_LIB

SOURCES += Channel.cpp ADC.cpp ADCUtil.cpp Command.cpp Reply.cpp ADCRepo.cpp

HEADERS += Channel.h IADC.h ADC.h ADCUtil.h Command.h Reply.h ADC_global.h ADCRepo.h

DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml

unix: LIBS += \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src
DEPENDPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src
               

RESOURCES += ADC.xml
