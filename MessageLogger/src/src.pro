include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = MessageLoggerServer

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += MessageLogger.xml
DBUS_INTERFACES += MessageLogger.xml

QT += sql widgets network dbus

HEADERS += MessageLoggerServer.h

SOURCES += MessageLoggerServer.cpp main.cpp

unix: LIBS += \
-L$$OUT_PWD/../../Global/src/ -lGlobal \

INCLUDEPATH += \
$$PWD/../../Global/src

DEPENDPATH += \
$$PWD/../../Global/src

