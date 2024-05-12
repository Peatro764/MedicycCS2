include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = Utils

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += core

HEADERS +=  \
EquipmentGroupState.h

SOURCES += \
EquipmentGroupState.cpp

unix: LIBS +=

INCLUDEPATH +=

DEPENDPATH +=
