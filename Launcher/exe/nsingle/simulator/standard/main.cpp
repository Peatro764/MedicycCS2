#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include "Parameters.h"
#include "DBus.h"
#include "StandardNSingleSimulator.h"
#include "NSingleRepo.h"
#include "Controller.h"
#include "nsingle_adaptor.h"
#include "nsinglecontroller_adaptor.h"
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>
#include "VirtualNSingle.h"


namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./StandardNSingleDBus \"Dipole M1\"";
        return -1;
    }

    QString name = arguments.at(1);
    qDebug() << "Setting up NSingle " << name;

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
    medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo repo("_prod");

    try {
        qDebug() << "Creating nsingle and register on the dbus";
        hw_nsingle::NSingleConfig config = repo.GetNSingleConfig(name);
        hw_nsingle::VirtualNSingle nsingle(config);
        hw_nsingle::Controller controller(&nsingle, 0);

        hw_nsingle::NSingleDbusAddress dbus_address =  repo.GetNSingleDbusAddress(name);
        new NSingleInterfaceAdaptor(&nsingle);
        if(dbus_connection.registerObject(dbus_address.nsingle_object_name(), &nsingle)) {
            qDebug() << "Successfully registered nsingle object on dbus";
        } else {
            qWarning() << "Failed registering nsingle object on dbus: " << dbus_connection.lastError().message();
        }
        new NSingleControllerInterfaceAdaptor(&controller);
        if (dbus_connection.registerObject(dbus_address.controller_object_name(), &controller)) {
            qDebug() << "Successfully registered object on dbus";
        } else {
            qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
        }
        if(dbus_connection.registerService(dbus_address.interface_address())) {
            qDebug() << "Successfully registered dbus service";
        } else {
            qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
        }

        qDebug() << "Application startup";
        medicyc::cyclotroncontrolsystem::simulators::nsingle::standard::StandardNSingleSimulator gui(&nsingle, 0);
        gui.show();
        return app.exec();
    } catch (std::exception& exc) {
        qWarning() << "Caught exception: " << exc.what() << "\nThe program will terminate";
        return -1;
    }

}


