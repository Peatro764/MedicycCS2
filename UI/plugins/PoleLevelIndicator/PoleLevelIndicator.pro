include(../../../MedicycCS2.pri)

QT += widgets uiplugin

QTDIR_build {

    PLUGIN_TYPE = designer
    PLUGIN_CLASS_NAME = PoleLevelIndicatorPlugin
    load(qt_plugin)
    CONFIG += install_ok

} else {

    TARGET = $$qtLibraryTarget($$TARGET)
    CONFIG += plugin
    TEMPLATE = lib

    target.path = $$[QT_INSTALL_PLUGINS]/designer
    INSTALLS += target

}

HEADERS = PoleLevelIndicator.h PoleLevelIndicatorPlugin.h

SOURCES = PoleLevelIndicator.cpp PoleLevelIndicatorPlugin.cpp

