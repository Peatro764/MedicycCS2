include(../../../MedicycCS2.pri)

QT += testlib sql

CONFIG += depend_includepath testcase

TEMPLATE = app

TARGET = TestNSingle

HEADERS += TestNSingleRepo.h TestRegulatedParameter.h TestMultiplexConfig.h TestConfig.h TestMeasurement.h \
    TestError.h TestCommand.h TestStateReply.h TestNSingleUtil.h TestStateCommand.h TestReply.h
    #TestNSingleUtil.h TestCommand.h TestReply.h

SOURCES += TestNSingleRepo.cpp TestRegulatedParameter.cpp TestMultiplexConfig.cpp TestConfig.cpp TestMeasurement.cpp \
    TestError.cpp TestCommand.cpp TestStateReply.cpp TestNSingleUtil.cpp TestStateCommand.cpp \
    TestReply.cpp testmain.cpp
    #TestNSingleUtil.cpp TestCommand.cpp TestReply.cpp

unix: LIBS += \
    -L$$OUT_PWD/../src/ -lNSingle \
    -L$$OUT_PWD/../../ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../../Global/src/ -lGlobal

INCLUDEPATH += \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

DEPENDPATH += \
    $$PWD/../src \
    $$PWD/../../ClientConnection/src \
    $$PWD/../../../Global/src

RESOURCES = ../resources.qrc
