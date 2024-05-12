#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"
#include "RadiationMonitor.h"
#include "RadiationMonitorRepo.h"
#include "radiationmonitor_adaptor.h"

namespace hw_radiationmonitor = medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./RadiationMonitor [name]";
        return -1;
    }

    QString name = QCoreApplication::arguments().at(1);
    qDebug() << "Setting up radiaton monitor " << name;

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    qDebug() << "Connecting to database";
    hw_radiationmonitor::RadiationMonitorRepo repo("_prod");

    try {
        hw_radiationmonitor::Configuration configuration = repo.GetConfiguration(name);
        hw_radiationmonitor::RadiationMonitor radiation_monitor(configuration);

        new RadiationMonitorInterfaceAdaptor(&radiation_monitor);
        if (dbus_connection.registerObject("/" + name, &radiation_monitor)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus";
            return -1;
        }
        if (dbus_connection.registerService("medicyc.cyclotron.hardware.radiation_monitor." + name)) {
            qDebug() << "Successfully registered dbus service";
        } else {
            qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
            return -1;
        }
        qDebug() << "Application startup";
        return app.exec();
    } catch(std::exception& exc) {
        qDebug() << "Failed setting up radiation monitor: Exception caught " << exc.what();
        return -1;
    }
}


