include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = Gui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus

HEADERS +=  Gui.h

SOURCES += Gui.cpp main.cpp

FORMS += Gui.ui

DBUS_INTERFACES += ../src/MessageLogger.xml

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
-L$$OUT_PWD/../../Global/src/ -lGlobal \

INCLUDEPATH += \
$$PWD/../../Global/src

DEPENDPATH += \
$$PWD/../../Global/src

RESOURCES +=
