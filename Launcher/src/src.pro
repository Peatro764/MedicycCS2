include(../../MedicycCS2.pri)

QT += network sql widgets dbus

TARGET = Launcher

TEMPLATE = app

DBUS_ADAPTORS += Launcher.xml

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += LAUNCHER_LIB

SOURCES += Launcher.cpp Process.cpp Library.cpp main.cpp

HEADERS += Launcher.h Library.h Process.h

unix: RESOURCES += ../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../Global/src

DEPENDPATH += \
    $$PWD/../../Global/src

