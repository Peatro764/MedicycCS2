#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QtDBus/QDBusConnection>
#include <QtWidgets/QApplication>
#include <QtDBus>

#include "StandardNSingleGroup.h"
#include "nsinglegroup_adaptor.h"

#include "Parameters.h"
#include "DBus.h"
#include "Enumerations.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (QCoreApplication::arguments().size() != 3) {
        qWarning() << "Wrong number of arguments given. Format: ./NSingleGroup \"VDF\" 3000";
        return -1;
    }
    QString sub_system = QCoreApplication::arguments().at(1);
    bool ok(false);
    int iteration_time = QCoreApplication::arguments().at(2).toInt(&ok);
    if (!ok) {
        qDebug() << "A correct iteration time as not given: " << QCoreApplication::arguments().at(2);
    }
    qDebug() << "Setting up NSingleGroup " << sub_system << " with iteration time " << iteration_time;


    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::middlelayer::nsinglegroup::standard::StandardNSingleGroup nsingle_group(sub_system, iteration_time);
    new NSingleGroupInterfaceAdaptor(&nsingle_group);
    if (dbus_connection.registerObject("/" + sub_system, &nsingle_group)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
    }

    if (dbus_connection.registerService("medicyc.cyclotron.middlelayer." + sub_system.toLower())) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
    }

    qDebug() << "Application startup";
    return app.exec();
}


