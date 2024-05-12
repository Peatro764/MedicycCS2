#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Electrometer.h"
#include "electrometer_adaptor.h"
#include "DBus.h"
#include "Parameters.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./Electrometer [name]";
        return -1;
    }

    QString name = QCoreApplication::arguments().at(1);
    qDebug() << "Setting up electrometer " << name;

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::hardware::electrometer::Electrometer electrometer;
    new ElectrometerInterfaceAdaptor(&electrometer);
    if (dbus_connection.registerObject("/" + name, &electrometer)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus";
        return -1;
    }
    if (dbus_connection.registerService("medicyc.cyclotron.hardware.electrometer." + name)) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
        return -1;
    }
    qDebug() << "Application startup";
    return app.exec();
}


