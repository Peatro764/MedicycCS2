include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = SondePrincipale

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus charts printsupport multimedia

HEADERS +=  SondePrincipale.h

SOURCES += SondePrincipale.cpp main.cpp

FORMS +=

DBUS_INTERFACES += \
    $$PWD/../../Hardware/ADC/src/ADC.xml

unix: RESOURCES += ../../Styles/breeze_unix.qrc

LIBS += \
    -L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
    -L$$OUT_PWD/../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../../External/qcustomplot \
    $$PWD/../../Global/src

DEPENDPATH += \
    $$PWD/../../External/qcustomplot \
    $$PWD/../../Global/src
