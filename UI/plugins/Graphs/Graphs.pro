include(../../../MedicycCS2.pri)

QT += widgets charts

TEMPLATE = lib
TARGET = Graphs

HEADERS += Histogram.h
SOURCES += Histogram.cpp

RESOURCES +=
    
DEFINES += GRAPHS_LIBRARY

LIBS += \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot

INCLUDEPATH += \
$$PWD/../../../External/qcustomplot

DEPENDPATH += \
$$PWD/../../../External/qcustomplot
