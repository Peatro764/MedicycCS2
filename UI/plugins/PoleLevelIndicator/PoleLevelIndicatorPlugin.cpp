#include "PoleLevelIndicator.h"
#include "PoleLevelIndicatorPlugin.h"

#include <QtPlugin>

PoleLevelIndicatorPlugin::PoleLevelIndicatorPlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void PoleLevelIndicatorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool PoleLevelIndicatorPlugin::isInitialized() const
{
    return initialized;
}

QWidget *PoleLevelIndicatorPlugin::createWidget(QWidget *parent)
{
    return new PoleLevelIndicator(parent);
}

QString PoleLevelIndicatorPlugin::name() const
{
    return "PoleLevelIndicator";
}

QString PoleLevelIndicatorPlugin::group() const
{
    return "Display Widgets";
}

QIcon PoleLevelIndicatorPlugin::icon() const
{
    return QIcon();
}

QString PoleLevelIndicatorPlugin::toolTip() const
{
    return "";
}

QString PoleLevelIndicatorPlugin::whatsThis() const
{
    return "";
}

bool PoleLevelIndicatorPlugin::isContainer() const
{
    return false;
}

QString PoleLevelIndicatorPlugin::domXml() const
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

QString PoleLevelIndicatorPlugin::includeFile() const
{
    return "PoleLevelIndicator.h";
}
