include(../../../MedicycCS2.pri)

QT += core \
    gui \
    network \
    sql \
    testlib \
    widgets
CONFIG += qt warn_on depend_includepath testcase
LIBS += -L/usr/local/lib
TEMPLATE = app
TARGET = TestRadiationMonitor
HEADERS += TestMeasurementRingBuffer.h TestMeasurement.h TestXRayIdentifier.h TestRadiationMonitorRepo.h
SOURCES += TestMeasurementRingBuffer.cpp TestMeasurement.cpp TestXRayIdentifier.cpp TestRadiationMonitorRepo.cpp testmain.cpp

RESOURCES = resources.qrc

unix: LIBS += \
-L$$OUT_PWD/../src/ -lRadiationMonitor \
-L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
-L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot \
-L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH +=\
$$PWD/../src \
$$PWD/../../ClientConnection/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Global/src

DEPENDPATH += \
$$PWD/../src \
$$PWD/../../ClientConnection/src \
$$PWD/../../../External/qcustomplot \
$$PWD/../../../Global/src
