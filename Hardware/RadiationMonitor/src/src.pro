include(../../../MedicycCS2.pri)

QT += core network sql dbus

TARGET = RadiationMonitor

TEMPLATE = lib

RESOURCES += resources.qrc RadiationMonitor.xml

DBUS_INTERFACES += $$PWD/../../../MessageLogger/src/MessageLogger.xml

DEFINES += RadiationMonitoring_LIB

SOURCES += RadiationMonitor.cpp RadiationMonitorRepo.cpp Configuration.cpp Measurement.cpp ConfigurationComparator.cpp Command.cpp ReplyParser.cpp CommandCreator.cpp MeasurementRingBuffer.cpp XRayIdentifier.cpp

HEADERS += Definitions.h RadiationMonitor.h RadiationMonitorRepo.h ConfigurationComparator.h Measurement.h Command.h ReplyParser.h CommandCreator.h MeasurementRingBuffer.h XRayIdentifier.h Configuration.h

unix: LIBS += \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../../../External/qcustomplot/ -lqcustomplot

INCLUDEPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src \
    $$PWD/../../../External/qcustomplot

DEPENDPATH += \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src \
    $$PWD/../../../External/qcustomplot

