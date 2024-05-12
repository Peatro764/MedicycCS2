#include "HorisontalLevelBars.h"
#include "HorisontalLevelBarsPlugin.h"

#include <QtPlugin>

HorisontalLevelBarsPlugin::HorisontalLevelBarsPlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void HorisontalLevelBarsPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool HorisontalLevelBarsPlugin::isInitialized() const
{
    return initialized;
}

QWidget *HorisontalLevelBarsPlugin::createWidget(QWidget *parent)
{
    return new HorisontalLevelBars(parent);
}

QString HorisontalLevelBarsPlugin::name() const
{
    return "HorisontalLevelBars";
}

QString HorisontalLevelBarsPlugin::group() const
{
    return "Display Widgets";
}

QIcon HorisontalLevelBarsPlugin::icon() const
{
    return QIcon();
}

QString HorisontalLevelBarsPlugin::toolTip() const
{
    return "";
}

QString HorisontalLevelBarsPlugin::whatsThis() const
{
    return "";
}

bool HorisontalLevelBarsPlugin::isContainer() const
{
    return false;
}

QString HorisontalLevelBarsPlugin::domXml() const
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

QString HorisontalLevelBarsPlugin::includeFile() const
{
    return "HorisontalLevelBars.h";
}
