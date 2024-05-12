#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "ElectrometerGui.h"

#include "Parameters.h"
#include "DBus.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    medicyc::cyclotroncontrolsystem::ui::electrometer::ElectrometerGui gui;
    gui.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    gui.show();
    return app.exec();
}


