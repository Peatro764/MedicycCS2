#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "OmronRepo.h"
#include "CoolingSystem.h"
#include "coolingsystem_adaptor.h"

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    qDebug() << "Setting up coolingsystem";

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    try {
        hw_omron::OmronRepo repo("_prod");
        if (!repo.Connect(5)) {
            qWarning() << "Connection to database failed";
        }

        qDebug() << "Creating source and register on the dbus";
        auto channels = repo.GetChannels("Cooling");
        if (channels.empty()) {
            throw std::runtime_error("No omron cooling channels found");
        }
        medicyc::cyclotroncontrolsystem::middlelayer::coolingsystem::CoolingSystem cooling(channels.keys());

        new CoolingSystemInterfaceAdaptor(&cooling);
        if (dbus_connection.registerObject("/CoolingSystem", &cooling)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        QString service_name = QString("medicyc.cyclotron.middlelayer.coolingsystem");
        if (dbus_connection.registerService(service_name)) {
            qDebug() << "Successfully registered dbus service";
        } else {
            qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
            return -1;
        }

        qDebug() << "Application startup";
        return app.exec();
    } catch (std::exception& exc) {
        qWarning() << "Caught exception: " << exc.what() << "\nThe program will terminate";
        return -1;
    }

}


