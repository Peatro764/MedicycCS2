include(../../MedicycCS2.pri)

TARGET = Global

TEMPLATE = lib

QT += core dbus sql

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += GLOBAL_LIBRARY

SOURCES += TimedState.cpp CountedState.cpp CountedTimedState.cpp MonitoredState.cpp \
    Calc.cpp \
    DBus.cpp \
    Parameters.cpp \
    GlobalRepo.cpp

HEADERS += Enumerations.h ThreadSafeQueue.h TimedState.h CountedState.h CountedTimedState.h MonitoredState.h \
    Calc.h \
    DBus.h \
    Parameters.h \
    GlobalRepo.h

unix: LIBS +=

INCLUDEPATH +=

DEPENDPATH +=
