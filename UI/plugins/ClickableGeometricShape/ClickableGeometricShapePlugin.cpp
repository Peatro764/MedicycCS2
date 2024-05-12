#include "ClickableGeometricShape.h"
#include "ClickableGeometricShapePlugin.h"

#include <QtPlugin>

ClickableGeometricShapePlugin::ClickableGeometricShapePlugin(QObject *parent)
    : QObject(parent)
    , initialized(false)
{
}

void ClickableGeometricShapePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (initialized)
        return;

    initialized = true;
}

bool ClickableGeometricShapePlugin::isInitialized() const
{
    return initialized;
}

QWidget *ClickableGeometricShapePlugin::createWidget(QWidget *parent)
{
    return new ClickableGeometricShape(parent);
}

QString ClickableGeometricShapePlugin::name() const
{
    return "ClickableGeometricShape";
}

QString ClickableGeometricShapePlugin::group() const
{
    return "Display Widgets";
}

QIcon ClickableGeometricShapePlugin::icon() const
{
    return QIcon();
}

QString ClickableGeometricShapePlugin::toolTip() const
{
    return "";
}

QString ClickableGeometricShapePlugin::whatsThis() const
{
    return "";
}

bool ClickableGeometricShapePlugin::isContainer() const
{
    return false;
}

QString ClickableGeometricShapePlugin::domXml() const
{
    return "<ui language=\"c++\">\n"
           " <widget class=\"Stepper\" name=\"ClickableGeometricShape\">\n"
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

QString ClickableGeometricShapePlugin::includeFile() const
{
    return "Stepper.h";
}

