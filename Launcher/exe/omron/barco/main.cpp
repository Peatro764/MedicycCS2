#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "OmronRepo.h"
#include "Barco.h"
#include "barco_adaptor.h"

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    qDebug() << "Setting up BARCO";

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
    hw_omron::OmronRepo repo("_prod");
    if (!repo.Connect(5)) {
        qWarning() << "Connection to database failed";
    }

    try {
        qDebug() << "Creating omron register on the dbus";
        hw_omron::Barco barco;
        auto messages = repo.GetBarcoMessages();
        barco.SetMessages(messages);

        new BarcoInterfaceAdaptor(&barco);
        if (dbus_connection.registerObject("/Barco", &barco)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        QString service_name = QString("medicyc.cyclotron.hardware.barco");
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


