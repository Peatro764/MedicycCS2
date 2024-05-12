include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = BeamlineGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus multimedia

HEADERS += Gui.h Scene.h BeamPoint.h BeamPath.h BeamLine.h Constants.h BeamItem.h CF.h Dipole.h QZ.h Room.h Quadrupole.h Cyclotron.h DT.h Message.h

SOURCES += Gui.cpp Scene.cpp BeamPoint.cpp BeamPath.cpp BeamLine.cpp main.cpp BeamItem.cpp CF.cpp Dipole.cpp QZ.cpp Room.cpp Quadrupole.cpp Cyclotron.cpp DT.cpp

FORMS +=

DBUS_INTERFACES += \
    $$PWD/../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../MiddleLayer/BeamLineState/BeamLineState.xml

unix: RESOURCES += resources.qrc ../../Styles/breeze_unix.qrc

macx:CONFIG(debug, debug|release): LIBS += \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton_debug \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../shared/ -lShared
else:unix: LIBS += \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../shared/ -lShared

INCLUDEPATH += \
    $$PWD/../shared/ \
    $$PWD/../../Global/src \

DEPENDPATH += \
    $$PWD/../shared/ \
    $$PWD/../../Global/src

RESOURCES += resources.qrc
