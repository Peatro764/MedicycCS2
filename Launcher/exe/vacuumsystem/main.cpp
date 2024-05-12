#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "IORRepo.h"
#include "VacuumSystem.h"
#include "vacuumsystem_adaptor.h"

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    qDebug() << "Setting up vacuumsystem";

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
        hw_ior::IORRepo repo("_prod");
        if (!repo.Connect(5)) {
            qWarning() << "Connection to database failed";
        }

        qDebug() << "Creating vacuumsystem and register on the dbus";
        auto channels = repo.GetIORChannelNames("Vacuum");
        if (channels.empty()) {
            throw std::runtime_error("No ior vacuum channels found");
        }
        medicyc::cyclotroncontrolsystem::middlelayer::vacuumsystem::VacuumSystem vacuum(channels);

        new  VacuumSystemInterfaceAdaptor(&vacuum);
        if (dbus_connection.registerObject("/VacuumSystem", &vacuum)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        QString service_name = QString("medicyc.cyclotron.middlelayer.vacuumsystem");
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


