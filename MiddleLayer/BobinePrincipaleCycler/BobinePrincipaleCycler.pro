include(../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = BobinePrincipaleCycler

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

HEADERS += BobinePrincipaleCycler.h

SOURCES += BobinePrincipaleCycler.cpp

DBUS_INTERFACES += \
    $$PWD/../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../MessageLogger/src/MessageLogger.xml

DBUS_ADAPTORS += $$PWD/BobinePrincipaleCycler.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Global/src/

DEPENDPATH += \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Global/src/

