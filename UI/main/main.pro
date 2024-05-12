include(../../MedicycCS2.pri)

TEMPLATE = app

TARGET = MainGui

# DEFINES += QT_NO_DEBUG_OUTPUT

QT += gui sql network widgets dbus charts printsupport multimedia

HEADERS +=  MainGui.h SourceUI.h HFUI.h NSingleGroupUI.h ProcessMonitorUI.h ConfigSelectionDialog.h ConfigSaveDialog.h \
    BobinePrincipaleUI.h EquipmentGroupValues.h IOLoadUI.h ISubSystemUI.h \
    PrintWidget.h StateLabel.h MessageUI.h CoolingUI.h HFPowerSupplyDetailedUI.h HFConfigurationUI.h VacuumUI.h \
    SoundMessages.h OmronSelectionDialog.h

SOURCES += MainGui.cpp SourceUI.cpp HFUI.cpp ConfigSelectionDialog.cpp main.cpp NSingleGroupUI.cpp ProcessMonitorUI.cpp \
    ConfigSaveDialog.cpp BobinePrincipaleUI.cpp EquipmentGroupValues.cpp IOLoadUI.cpp \
    PrintWidget.cpp StateLabel.cpp MessageUI.cpp CoolingUI.cpp HFPowerSupplyDetailedUI.cpp HFConfigurationUI.cpp \
    VacuumUI.cpp SoundMessages.cpp OmronSelectionDialog.cpp

FORMS += ConfigSelectionDialog.ui ConfigSaveDialog.ui OmronSelectionDialog.ui

DBUS_INTERFACES += \
    $$PWD/../../Hardware/NSingle/src/NSingleController.xml \
    $$PWD/../../Hardware/NSingle/src/NSingle.xml \
    $$PWD/../../Hardware/ADC/src/ADC.xml \
    $$PWD/../../Hardware/IOR/src/IOR.xml \
    $$PWD/../../Hardware/Omron/src/Omron.xml \
    $$PWD/../../Hardware/SourcePowerSupply/src/SourcePowerSupply.xml \
    $$PWD/../../MiddleLayer/Source/Source.xml \
    $$PWD/../../Hardware/HFPowerSupplies/src/HFPowerSupplies.xml \
    $$PWD/../../MiddleLayer/HF/src/HFController.xml \
    $$PWD/../../MiddleLayer/NSingleGroup/NSingleGroup.xml \
    $$PWD/../../MiddleLayer/BobinePrincipaleCycler/BobinePrincipaleCycler.xml \
    $$PWD/../../MiddleLayer/CoolingSystem/CoolingSystem.xml \
    $$PWD/../../MiddleLayer/VacuumSystem/VacuumSystem.xml \
    $$PWD/../../MessageLogger/src/MessageLogger.xml \
    $$PWD/../../Launcher/src/Launcher.xml

unix: RESOURCES += ../../Styles/breeze_unix.qrc

macx:CONFIG(debug, debug|release): LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton_debug \
    -L$$OUT_PWD/../plugins/PoleLevelIndicator/ -lPoleLevelIndicator_debug \
    -L$$OUT_PWD/../plugins/FancyDisplay/ -lFancyDisplay_debug \
    -L$$OUT_PWD/../plugins/ClickableGeometricShape/ -lClickableGeometricShape_debug \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../../MiddleLayer/HF/src/ -lHF \
    -L$$OUT_PWD/../../Hardware/IOR/src/ -lIOR \
    -L$$OUT_PWD/../shared/ -lShared \
    -L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
    -L/usr/local/qwt/lib
else:unix: LIBS += \
    -L$$OUT_PWD/../../Global/src/ -lGlobal \
    -L$$OUT_PWD/../plugins/FancyDisplay/ -lFancyDisplay \
    -L$$OUT_PWD/../plugins/GenericDisplayButton/ -lGenericDisplayButton \
    -L$$OUT_PWD/../plugins/PoleLevelIndicator/ -lPoleLevelIndicator \
    -L$$OUT_PWD/../plugins/ClickableGeometricShape/ -lClickableGeometricShape \
    -L$$OUT_PWD/../../Hardware/NSingle/src/ -lNSingle \
    -L$$OUT_PWD/../../Hardware/SourcePowerSupply/src/ -lSourcePowerSupply \
    -L$$OUT_PWD/../../Hardware/ClientConnection/src/ -lClientConnection \
    -L$$OUT_PWD/../../MiddleLayer/HF/src/ -lHF \
    -L$$OUT_PWD/../../Hardware/Omron/src/ -lOmron \
    -L$$OUT_PWD/../../Hardware/IOR/src/ -lIOR \
    -L$$OUT_PWD/../shared/ -lShared \
    -L$$OUT_PWD/../../External/qcustomplot/ -lqcustomplot \
    -L/usr/local/qwt/lib/

INCLUDEPATH += \
    $$PWD/../../Global/src \
    $$PWD/../shared \
    $$PWD/../plugins/GenericDisplayButton \
    $$PWD/../plugins/PoleLevelIndicator \
    $$PWD/../plugins/ClickableGeometricShape \
    $$PWD/../../MiddleLayer/HF/src/ \
    $$PWD/../plugins/FancyDisplay \
    $$PWD/../../Hardware/SourcePowerSupply/src/ \
    $$PWD/../../Hardware/ClientConnection/src \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../Global/src \
    $$PWD/../../External/qcustomplot \
    /usr/local/qwt/include

DEPENDPATH += \
    $$PWD/../../Global/src \
    $$PWD/../shared \
    $$PWD/../plugins/GenericDisplayButton \
    $$PWD/../plugins/PoleLevelIndicator \
    $$PWD/../plugins/ClickableGeometricShape \
    $$PWD/../plugins/FancyDisplay \
    $$PWD/../../Hardware/ClientConnection/src \
    $$PWD/../../MiddleLayer/HF/src/ \
    $$PWD/../../Hardware/NSingle/src \
    $$PWD/../../Hardware/SourcePowerSupply/src/ \
    $$PWD/../../Hardware/Omron/src \
    $$PWD/../../Hardware/IOR/src \
    $$PWD/../../CycloConfig/src \
    $$PWD/../../Global/src \
    $$PWD/../../External/qcustomplot \
    /usr/local/qwt/include

RESOURCES += resources.qrc
