include(../../../MedicycCS2.pri)

TEMPLATE = lib

TARGET = Raspberry

QT += gui network sql widgets

HEADERS +=  GPIOHandler.h

SOURCES += GPIOHandler.cpp

FORMS +=

# TODO put software in appropriate directory and change paths below
unix: LIBS += \
    -L/home/raspberry/software/WiringPi/wiringPi/ -lwiringPi \
    -L/home/raspberry/software/WiringPi/devLib/ -lwiringPiDev

INCLUDEPATH += \
    /home/raspberry/software/WiringPi/wiringPi \
    /home/raspberry/software/WiringPi/devLib

DEPENDPATH += \

RESOURCES +=

