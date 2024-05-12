include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = BobinePrincipaleCycler

# DEFINES += QT_NO_DEBUG_OUTPUT

DBUS_ADAPTORS += $$PWD/../../../MiddleLayer/BobinePrincipaleCycler/BobinePrincipaleCycler.xml
DBUS_INTERFACES += \
$$PWD/../../../MessageLogger/src/MessageLogger.xml \
$$PWD/../../../Hardware/NSingle/src/NSingleController.xml

QT += sql widgets network dbus

HEADERS +=
SOURCES += main.cpp

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../MiddleLayer/BobinePrincipaleCycler/ -lBobinePrincipaleCycler

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/BobinePrincipaleCycler/

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src \
    $$PWD/../../../MiddleLayer/BobinePrincipaleCycler/

