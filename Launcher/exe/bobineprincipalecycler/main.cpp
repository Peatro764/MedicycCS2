#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QtDBus/QDBusConnection>
#include <QtWidgets/QApplication>
#include <QtDBus>

#include "Parameters.h"
#include "DBus.h"
#include "BobinePrincipaleCycler.h"
#include "bobineprincipalecycler_adaptor.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::middlelayer::bobine_principale_cycler::BobinePrincipaleCycler bp_cycler;
    new BobinePrincipaleCyclerInterfaceAdaptor(&bp_cycler);
    if (dbus_connection.registerObject("/BobinePrincipaleCycler", &bp_cycler)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
    }
    if (dbus_connection.registerService("medicyc.cyclotron.middlelayer.bobine_principale_cycler")) {
        qDebug() << "Successfully registered service on dbus";
    } else {
        qWarning() << "Failed registering service on dbus: " << dbus_connection.lastError().message();
    }

    qDebug() << "Application startup";
    return app.exec();
}


