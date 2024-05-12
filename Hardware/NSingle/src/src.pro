include(../../../MedicycCS2.pri)

QT += network sql dbus

TARGET = NSingle

TEMPLATE = lib

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += NSINGLE_LIB

SOURCES += NSingleRepo.cpp RegulatedParameter.cpp Controller.cpp MultiplexConfig.cpp MultiplexedNSingle.cpp \
    ReplyParser.cpp CommandCreator.cpp Config.cpp NSingle.cpp NSingleUtil.cpp Command.cpp \
    Reply.cpp Measurement.cpp Error.cpp AddressRegister.cpp StateCommand.cpp NSingleConfig.cpp \
    StateReply.cpp Bit.cpp VirtualNSingle.cpp NSingleSetPoint.cpp

HEADERS += NSingleRepo.h Controller.h INSingle.h RegulatedParameter.h Controller.h MultiplexConfig.h \
    MultiplexedNSingle.h ReplyParser.h CommandCreator.h Config.h NSingle.h NSingleUtil.h \
    Command.h Reply.h Measurement.h Error.h AddressRegister.h StateCommand.h NSingleConfig.h \
    Bit.h StateReply.h NSingle_global.h VirtualNSingle.h NSingleDbusAddress.h NSingleSetPoint.h

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

RESOURCES += NSingleController.xml \
             NSingle.xml
