include(../../../MedicycCS2.pri)

QT += network sql dbus

TARGET = Omron

TEMPLATE = lib

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += OMRON_LIB

SOURCES += Channel.cpp OmronRepo.cpp FINS.cpp Omron.cpp Barco.cpp MemoryAreaCommand.cpp NodeAddressCommand.cpp \
    FrameSendCommand.cpp BarcoMessage.cpp

HEADERS += Channel.h IOmron.h OmronRepo.h FINS.h Omron.h Barco.h MemoryAreaCommand.h NodeAddressCommand.h \
    FrameSendCommand.h BarcoMessage.h Omron_global.h

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

