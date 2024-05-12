include(../../../MedicycCS2.pri)

QT += network serialport

# DEFINES += QT_NO_DEBUG_OUTPUT

TARGET = ClientConnection

TEMPLATE = lib

DEFINES += CLIENTCONNECTION_LIBRARY

SOURCES += SerialClient.cpp SocketClient.cpp Util.cpp

HEADERS += ClientConnection.h SerialClient.h SocketClient.h Util.h

