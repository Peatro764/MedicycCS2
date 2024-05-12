include(../../../MedicycCS2.pri)

TEMPLATE = app

TARGET = RaspberryGui

QT += gui network sql widgets dbus

HEADERS +=  raspberry.h

SOURCES += raspberry.cpp main.cpp

FORMS += raspberry.ui

unix: RESOURCES += ../../../Styles/breeze_unix.qrc

unix: LIBS += \
    -L$$OUT_PWD/../src/ -lRaspberry \
#    -L/home/raspberry/software/WiringPi/wiringPi/ -lwiringPi \
#    -L/home/raspberry/software/WiringPi/devLib/ -lwiringPiDev

INCLUDEPATH += \
    $$PWD/../src
#    /home/raspberry/software/WiringPi/wiringPi \
#    /home/raspberry/software/WiringPi/devLib

DEPENDPATH += \
    $$PWD/../src

RESOURCES += 

