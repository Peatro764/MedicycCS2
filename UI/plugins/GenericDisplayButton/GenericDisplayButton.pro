include(../../../MedicycCS2.pri)

QT += widgets uiplugin

QTDIR_build {

    PLUGIN_TYPE = designer
    PLUGIN_CLASS_NAME = GenericDisplayButtonPlugin
    load(qt_plugin)
    CONFIG += install_ok

} else {

    TARGET = $$qtLibraryTarget($$TARGET)
    CONFIG += plugin
    TEMPLATE = lib
    target.path = $$[QT_INSTALL_PLUGINS]/designer
    INSTALLS += target

}

HEADERS = GenericDisplayButton.h GenericDisplayButtonPlugin.h

SOURCES = GenericDisplayButton.cpp GenericDisplayButtonPlugin.cpp
