#include "FancyDisplay.h"
#include "FancyDisplayPlugin.h"

#include <QtPlugin>

FancyDisplayPlugin::FancyDisplayPlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void FancyDisplayPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool FancyDisplayPlugin::isInitialized() const
{
    return initialized;
}

QWidget *FancyDisplayPlugin::createWidget(QWidget *parent)
{
    return new FancyDisplay(parent);
}

QString FancyDisplayPlugin::name() const
{
    return "FancyDisplay";
}

QString FancyDisplayPlugin::group() const
{
    return "Display Widgets";
}

QIcon FancyDisplayPlugin::icon() const
{
    return QIcon();
}

QString FancyDisplayPlugin::toolTip() const
{
    return "";
}

QString FancyDisplayPlugin::whatsThis() const
{
    return "";
}

bool FancyDisplayPlugin::isContainer() const
{
    return false;
}

QString FancyDisplayPlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"Stepper\" name=\"FancyDisplay\">\n"
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

QString FancyDisplayPlugin::includeFile() const
{
    return "FancyDisplay.h";
}

