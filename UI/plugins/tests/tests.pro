include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = tests

QT += gui xml opengl widgets

HEADERS = tests.h

SOURCES = tests.cpp main.cpp

FORMS = tests.ui

RESOURCES +=

macx:CONFIG(debug, debug|release): LIBS += \
    -L$$OUT_PWD/../GenericDisplayButton/ -lGenericDisplayButton_debug \
    -L$$OUT_PWD/../PoleLevelIndicator/ -lPoleLevelIndicator_debug \
    -L$$OUT_PWD/../HorisontalLevelBars/ -lHorisontalLevelBars_debug \
    -L$$OUT_PWD/../ClickableGeometricShape/ -lClickableGeometricShape_debug \
    -L$$OUT_PWD/../FancyDisplay/ -lFancyDisplay_debug \
    -L$$OUT_PWD/../Graphs/ -lGraphs \
    -L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot
else:linux: LIBS += \
    -L$$OUT_PWD/../GenericDisplayButton/ -lGenericDisplayButton \
    -L$$OUT_PWD/../ClickableGeometricShape/ -lClickableGeometricShape \
    -L$$OUT_PWD/../FancyDisplay/ -lFancyDisplay \
    -L$$OUT_PWD/../HorisontalLevelBars/ -lHorisontalLevelBars \
    -L$$OUT_PWD/../PoleLevelIndicator/ -lPoleLevelIndicator \
    -L$$OUT_PWD/../Graphs/ -lGraphs \
    -L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot

INCLUDEPATH += \
    $$PWD/../GenericDisplayButton \
    $$PWD/../FancyDisplay \
    $$PWD/../ClickableGeometricShape \
    $$PWD/../HorisontalLevelBars \
    $$PWD/../PoleLevelIndicator \
    $$PWD/../Graphs \
    $$PWD/../../../External/qcustomplot/

DEPENDPATH += \
    $$PWD/../GenericDisplayButton \
    $$PWD/../FancyDisplay \
    $$PWD/../ClickableGeometricShape \
    $$PWD/../HorisontalLevelBars \
    $$PWD/../PoleLevelIndicator \
    $$PWD/../Graphs \
    $$PWD/../../../External/qcustomplot/


