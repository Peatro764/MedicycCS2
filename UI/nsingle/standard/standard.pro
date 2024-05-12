include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = StandardNSingleGui

QT += gui sql network widgets

HEADERS +=  NSingleGui.h NSingleWidget.h BitWidget.h BitFieldWidget.h \
    ClickableFrame.h

SOURCES += NSingleGui.cpp NSingleWidget.cpp BitWidget.cpp BitFieldWidget.cpp main.cpp \
    ClickableFrame.cpp

FORMS += NSingleGui.ui

unix: RESOURCES += ../../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../Hardware/NSingle/src -lNSingle \
    -L$$OUT_PWD/../../../Hardware/ClientConnection/src/ -lClientConnection

INCLUDEPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$OUT_PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src

DEPENDPATH += \
    $$PWD/../../../Global/src \
    $$PWD/../../../Hardware/NSingle/src \
    $$PWD/../../../Hardware/ClientConnection/src

RESOURCES += resources.qrc

   
   
