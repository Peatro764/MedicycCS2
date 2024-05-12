include(../../../MedicycCS2.pri)

QT += network sql dbus

TARGET = IOR

TEMPLATE = lib

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += IOR_LIB

RESOURCES += IOR.xml

SOURCES += Channel.cpp IORRepo.cpp IOR.cpp IORUtil.cpp Command.cpp Reply.cpp

HEADERS += Channel.h IORRepo.h IIOR.h IOR.h IORUtil.h Command.h Reply.h IOR_global.h

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

               
