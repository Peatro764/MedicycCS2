include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = Touch1Gui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus multimedia

HEADERS +=  EquipmentListGui.h \
    Touch1Gui.h ClickableLabel.h NumericInputDialog.h

SOURCES += EquipmentListGui.cpp main.cpp \
    Touch1Gui.cpp ClickableLabel.cpp NumericInputDialog.cpp

FORMS += NumericInputDialog.ui

DBUS_INTERFACES += \
    $$PWD/../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../Hardware/NSingle/src/NSingle.xml \
    $$PWD/../../Hardware/SourcePowerSupply/src/SourcePowerSupply.xml

unix: RESOURCES += ../../Styles/breeze_unix.qrc

# Include GPIO functionality only if running on a raspberry pi
equals(RASPBERRYPI,Pi) {
  message("Compiling for raspberry pi")
  DEFINES += RASPBERRY_PI
  LIBS += -L$$OUT_PWD/../../Hardware/Raspberry/src -lRaspberry
  INCLUDEPATH += $$PWD/../../Hardware/Raspberry/src
  DEPENDPATH+ += $$PWD/../../Hardware/Raspberry/src
}

macx:CONFIG(debug, debug|release): LIBS += \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton_debug \
    -L$$OUT_PWD/../plugins/PoleLevelIndicator/ -lPoleLevelIndicator_debug \
    -L$$OUT_PWD/../plugins/HorisontalLevelBars/ -lHorisontalLevelBars_debug \
    -L$$OUT_PWD/../plugins/ClickableGeometricShape/ -lClickableGeometricShape_debug \
    -L$$OUT_PWD/../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../shared/ -lShared
else:unix: LIBS += \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton \
    -L$$OUT_PWD/../plugins/PoleLevelIndicator/ -lPoleLevelIndicator \
    -L$$OUT_PWD/../plugins/HorisontalLevelBars/ -lHorisontalLevelBars \
    -L$$OUT_PWD/../plugins/ClickableGeometricShape/ -lClickableGeometricShape \
    -L$$OUT_PWD/../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../shared/ -lShared

INCLUDEPATH += \
    $$PWD/../plugins/GenericDisplayButton \
    $$PWD/../plugins/PoleLevelIndicator \
    $$PWD/../plugins/HorisontalLevelBars \
    $$PWD/../plugins/ClickableGeometricShape \
    $$PWD/../../Hardware/SourcePowerSupply/src \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/ClientConnection/src \
    $$PWD/../shared/ \
    $$PWD/../../Global/src \

DEPENDPATH += \
    $$PWD/../plugins/GenericDisplayButton \
    $$PWD/../plugins/PoleLevelIndicator \
    $$PWD/../plugins/HorisontalLevelBars \
    $$PWD/../plugins/ClickableGeometricShape \
    $$PWD/../../Hardware/SourcePowerSupply/src \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/ClientConnection/src \
    $$PWD/../shared/ \
    $$PWD/../../Global/src

RESOURCES += resources.qrc
