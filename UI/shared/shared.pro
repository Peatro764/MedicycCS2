include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = Shared

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += core widgets

HEADERS += Style.h Utils.h

SOURCES += Utils.cpp

macx:CONFIG(debug, debug|release): LIBS +=
macx:CONFIG(release, debug|release): LIBS +=
else:unix: LIBS +=

INCLUDEPATH +=

DEPENDPATH +=

