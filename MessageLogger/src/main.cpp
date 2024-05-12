#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "MessageLoggerServer.h"
#include "messagelogger_adaptor.h"
#include "DBus.h"
#include "Parameters.h"

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

    medicyc::cyclotroncontrolsystem::messagelogger::MessageLoggerServer server("_prod");
    new MessageLoggerInterfaceAdaptor(&server);
    if (dbus_connection.registerObject("/MessageLogger", &server)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
    }
    if (dbus_connection.registerService("medicyc.cyclotron.messagelogger")) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
    }

    qDebug() << "Start app";
    return app.exec();
}


