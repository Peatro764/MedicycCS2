#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include "Parameters.h"
#include "DBus.h"
#include "MultiplexedNSingleSimulator.h"
#include "NSingleRepo.h"
#include "Controller.h"
#include "nsinglecontroller_adaptor.h"
#include "nsingle_adaptor.h"
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
        qWarning() << "Wrong number of arguments given. Format: ./MultiplexedNSingleGui Multiplexed_1";
        return -1;
    }
    QString name = arguments.at(1);
    qDebug() << "Setting up multiplexed nsingle " << name;

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
        hw_nsingle::MultiplexConfig multiplex_config = repo.GetMultiplexConfig(name);

        for (auto& ch : multiplex_config.channels()) {
            qDebug() << "Channel " << ch;
            hw_nsingle::NSingleConfig ch_config = repo.GetNSingleConfig(ch);
            hw_nsingle::VirtualNSingle *v_nsingle = new hw_nsingle::VirtualNSingle(ch_config);
            hw_nsingle::NSingleDbusAddress ch_dbus_address =  repo.GetNSingleDbusAddress(ch);

            if (ch == multiplex_config.channels().at(0)) {
                new NSingleInterfaceAdaptor(v_nsingle);
                if (dbus_connection.registerObject(ch_dbus_address.nsingle_object_name(), v_nsingle)) {
                    qDebug() << "Successfully registered nsingle object on dbus";
                } else {
                    qWarning() << "Failed registering nsingle_object on dbus: " << dbus_connection.lastError().message();
                }
            }
            hw_nsingle::Controller *controller = new hw_nsingle::Controller(v_nsingle, 0);
            new NSingleControllerInterfaceAdaptor(controller);
            if (dbus_connection.registerObject(ch_dbus_address.controller_object_name(), controller)) {
                qDebug() << "Successfully registered nsingle controller object on dbus";
            } else {
                qWarning() << "Failed registering nsingle controller object on dbus: " << dbus_connection.lastError().message();
            }
            medicyc::cyclotroncontrolsystem::simulators::nsingle::multiplexed::MultiplexedNSingleSimulator *gui = new medicyc::cyclotroncontrolsystem::simulators::nsingle::multiplexed::MultiplexedNSingleSimulator(v_nsingle, 0);
            gui->show();
        }

        hw_nsingle::NSingleDbusAddress dbus_address =  repo.GetNSingleDbusAddress(name);
        if (dbus_connection.registerService(dbus_address.interface_address())) {
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


