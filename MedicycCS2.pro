include(MedicycCS2.pri)

SUBDIRS = Global Hardware UI MessageLogger Simulators DataLogger Launcher MiddleLayer External # Scanner

Hardware.depends = Global External MessageLogger
UI.depends = Hardware Global MiddleLayer External MessageLogger
MiddleLayer.depends = Global Hardware MessageLogger
MessageLogger.depends = Global
DataLogger.depends = Global Hardware MessageLogger
Simulators.depends = Global Hardware MessageLogger
Launcher.depends = Global Simulators Hardware MessageLogger MiddleLayer

RESOURCES += \
    Cyclotron.ini \
    Images/* \
    Styles/* \
    Scripts/* \
    XML/* \
    TODO.md

images.path = $$OUT_PWD/Images
images.files = Images
styles.path = $$OUT_PWD/Styles
styles.files = Styles

launch_scripts.path = $$OUT_PWD/LaunchScripts/
launch_scripts.files = LaunchScripts/*

INSTALLS += launch_scripts




  
 
