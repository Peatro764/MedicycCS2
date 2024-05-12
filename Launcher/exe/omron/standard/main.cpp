#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "OmronRepo.h"
#include "Omron.h"
#include "Barco.h"
#include "omron_adaptor.h"

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./Omron \"HF\"";
        return -1;
    }
    QString system = QCoreApplication::arguments().at(1);
    qDebug() << "Setting up OMRON " << system;

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
        hw_omron::Omron omron;
        auto channels = repo.GetChannels(system);
        omron.SetChannels(channels);

        new OmronInterfaceAdaptor(&omron);
        if (dbus_connection.registerObject("/Omron", &omron)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
            return -1;
        }
        QString service_name = QString("medicyc.cyclotron.hardware.omron.") + system.toLower();
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


