#include "GenericDisplayButton.h"
#include "GenericDisplayButtonPlugin.h"

#include <QtPlugin>

GenericDisplayButtonPlugin::GenericDisplayButtonPlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void GenericDisplayButtonPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool GenericDisplayButtonPlugin::isInitialized() const
{
    return initialized;
}

QWidget *GenericDisplayButtonPlugin::createWidget(QWidget *parent)
{
    return new GenericDisplayButton(parent);
}

QString GenericDisplayButtonPlugin::name() const
{
    return "GenericDisplayButton";
}

QString GenericDisplayButtonPlugin::group() const
{
    return "Display Widgets";
}

QIcon GenericDisplayButtonPlugin::icon() const
{
    return QIcon();
}

QString GenericDisplayButtonPlugin::toolTip() const
{
    return "";
}

QString GenericDisplayButtonPlugin::whatsThis() const
{
    return "";
}

bool GenericDisplayButtonPlugin::isContainer() const
{
    return false;
}

QString GenericDisplayButtonPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"GenericDisplayButton\" name=\"GenericDisplayButton\">\n"
           "  <property name=\"geometry\">\n"
           "   <rect>\n"
           "    <x>0</x>\n"
           "    <y>0</y>\n"
           "    <width>100</width>\n"
           "    <height>100</height>\n"
           "   </rect>\n"
           "  </property>\n"
           " </widget>\n"
           "</ui>";
}

QString GenericDisplayButtonPlugin::includeFile() const
{
    return "GenericDisplayButton.h";
}
