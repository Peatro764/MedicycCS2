#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "HFPowerSupplies.h"
#include "hfpowersupplies_adaptor.h"

namespace hfpowersupplies = medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    qDebug() << "Setting up HF PowerSupplies";

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
        qDebug() << "Creating source and register on the dbus";
        hfpowersupplies::HFPowerSupplies powersupplies;

        new HFPowerSuppliesInterfaceAdaptor(&powersupplies);
        if (dbus_connection.registerObject("/PowerSupplies", &powersupplies)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        QString service_name = QString("medicyc.cyclotron.hardware.hfpowersupplies");
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


