include(../../MedicycCS2.pri)

TEMPLATE = app
TARGET = Quartz

QT += core \
    gui \
    xml \
    network \
    opengl \
    widgets \
    sql

HEADERS = VideoWorker.h Quartz.h \
    AntialiasedRadioButton.h \
    BeamPlot.h \
    SpectrogramData.h \
    QuartzRepo.h \
    ImageSetSelectionDialog.h \
    ImageSetDefinitionDialog.h

SOURCES = VideoWorker.cpp Quartz.cpp main.cpp \
    AntialiasedRadioButton.cpp \
    BeamPlot.cpp \
    SpectrogramData.cpp \
    QuartzRepo.cpp \
    ImageSetSelectionDialog.cpp \
    ImageSetDefinitionDialog.cpp

FORMS = ImageSetSelectionDialog.ui \
        ImageSetDefinitionDialog.ui

RESOURCES += ../../Styles/breeze_unix.qrc

LIBS += \
    -lopencv_core -lopencv_videoio -lopencv_imgproc \
    -L/usr/local/qwt/lib/ \
    -L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../plugins/Graphs/ -lGraphs \
    -L$$OUT_PWD/../shared/ -lShared

macx:INCLUDEPATH += \
    /usr/local/include/opencv4/ \
    /usr/local/qwt/include

linux:INCLUDEPATH += \
    /usr/include/opencv4/ \
    /usr/local/qwt/include

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../External/qcustomplot \
    $$PWD/../plugins/Graphs/ \
    $$PWD/../shared/

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../../External/qcustomplot \
    $$PWD/../plugins/Graphs/

