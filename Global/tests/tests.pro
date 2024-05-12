include(../../MedicycCS2.pri)

QT += testlib sql

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestGlobal

RESOURCES += ../resources.qrc

HEADERS += TestQueue.h TestGlobalRepo.h

SOURCES += TestQueue.cpp TestGlobalRepo.cpp testmain.cpp


unix: LIBS += \
    -L$$OUT_PWD/../src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../src

DEPENDPATH += \
    $$PWD/../src

