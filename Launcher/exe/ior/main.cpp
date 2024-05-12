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

#include "IORRepo.h"
#include "IOR.h"
#include "ior_adaptor.h"

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Setting up IOR";

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
    hw_ior::IORRepo repo("_prod");
    if (!repo.Connect(5)) {
        qWarning() << "Connection to db failed, bailing out";
        return -1;
    }

    std::vector<hw_ior::Channel> channels;
    QStringList names;

    try {
        names = repo.GetIORChannelNames();

        for (QString& name : names) {
            channels.push_back(repo.GetIORChannel(name));
            qDebug() << "Found channel: " << name;
        }

        qDebug() << "Create IOR";
        hw_ior::IOR ior;
        ior.SetChannels(channels);

        new IORInterfaceAdaptor(&ior);
        if (dbus_connection.registerObject("/IOR", &ior)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
        }

        if (dbus_connection.registerService("medicyc.cyclotron.hardware.ior")) {
            qDebug() << "Successfully registered dbus service";
        } else {
            qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
        }

        qDebug() << "Application startup";
        return app.exec();
    } catch (std::exception& exc) {
        qWarning() << "Caught exception: " << exc.what() << "\nThe program will terminate";
        return -1;
    }
}
