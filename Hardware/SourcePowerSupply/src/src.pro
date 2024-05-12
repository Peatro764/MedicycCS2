include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = SourcePowerSupply

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += sql network dbus

RESOURCES += \
../resources.qrc \
SourcePowerSupply.xml

HEADERS +=  \
    PowerSupplyDbusAddress.h \
RegulatedParameter.h \
RegulatedParameterConfig.h \
SourcePowerSupply.h \
PowerSupplyConfig.h \
SourcePowerSupplyRepo.h

SOURCES += \
PowerSupplyDbusAddress.cpp \
RegulatedParameter.cpp \
RegulatedParameterConfig.cpp \
SourcePowerSupply.cpp \
PowerSupplyConfig.cpp \
SourcePowerSupplyRepo.cpp

DBUS_INTERFACES += \
    $$PWD/../../ADC/src/ADC.xml \
    $$PWD/../../Omron/src/Omron.xml \
    $$PWD/../../../MessageLogger/src/MessageLogger.xml

unix: LIBS += \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../../../Global/src
