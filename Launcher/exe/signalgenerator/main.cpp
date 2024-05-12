#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "DBus.h"
#include "Parameters.h"

#include "SignalGenerator.h"
#include "signalgenerator_adaptor.h"

namespace hw_signalgenerator = medicyc::cyclotroncontrolsystem::hardware::signalgenerator;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Setting up SignalGenerator";

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    qDebug() << "Create SignalGenerator";
    hw_signalgenerator::SignalGenerator generator;

    new SignalGeneratorInterfaceAdaptor(&generator);
    if (dbus_connection.registerObject("/SignalGenerator", &generator)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
        return -1;
    }
    if (dbus_connection.registerService("medicyc.cyclotron.hardware.signalgenerator")) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
        return -1;
    }

    qDebug() << "Start app";
    return app.exec();
}


