#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include "DataLogger.h"
#include "Parameters.h"
#include "DBus.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::datalogger::DataLogger logger("_prod");
    return app.exec();
}

