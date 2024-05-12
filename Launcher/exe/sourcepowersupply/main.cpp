#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "SourcePowerSupply.h"
#include "sourcepowersupply_adaptor.h"
#include "SourcePowerSupplyRepo.h"

namespace hw_sourcepowersupply = medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./SourcePowerSupply \"Extraction\"";
        return -1;
    }
    QString system = QCoreApplication::arguments().at(1);
    qDebug() << "Setting up SourcePowerSupply " << system;

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
    hw_sourcepowersupply::SourcePowerSupplyRepo repo("_prod");
    if (!repo.Connect(5)) {
        qWarning() << "Connection to database failed";
    }

    try {
        hw_sourcepowersupply::PowerSupplyConfig config = repo.GetConfig(system);

        qDebug() << "Creating source powersupply register on the dbus";
        hw_sourcepowersupply::SourcePowerSupply powersupply(config);

        hw_sourcepowersupply::PowerSupplyDbusAddress dbus_address = repo.GetDbusAddress(system);
        new SourcePowerSupplyInterfaceAdaptor(&powersupply);
        if (dbus_connection.registerObject(dbus_address.object_name(), &powersupply)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        if (dbus_connection.registerService(dbus_address.interface_address())) {
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


