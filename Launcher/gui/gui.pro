include(../../MedicycCS2.pri)

QT += network widgets sql dbus

TARGET = LauncherGui

TEMPLATE = app

SOURCES += LauncherGui.cpp main.cpp

HEADERS += LauncherGui.h

FORMS += LauncherGui.ui

DBUS_INTERFACES += $$PWD/../src/Launcher.xml

unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../Global/src

DEPENDPATH += \
    $$PWD/../../Global/src

unix: RESOURCES += ../../Styles/breeze_unix.qrc

