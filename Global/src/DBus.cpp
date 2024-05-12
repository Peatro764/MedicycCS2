#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::global {

QString GetDBusAddress(QString host, QString port) {
    QString address = QString("tcp:host=%1,port=%2").arg(host).arg(port);
    qDebug() << "DBUS connection\n"
             << "Host " << host << "\n"
             << "Port " << port << "\n"
             << "Full connection address " << address;
    return address;
}

QDBusConnection GetDBusConnection() {
    qDebug() << "Setting up dbus connection";
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              medicyc::cyclotroncontrolsystem::global::CONFIG_FILE,
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (!(settings.contains("dbus/host") && settings.contains("dbus/port") && settings.contains("dbus/name"))) {
        qWarning() << medicyc::cyclotroncontrolsystem::global::CONFIG_FILE << " does not contain all necessary dbus parameters (host, port, name), default values will be used";
    }
    QString dbus_address = GetDBusAddress(settings.value("dbus/host", "localhost").toString(), settings.value("dbus/port", "").toString());
    QString dbus_name = settings.value("dbus/name", "").toString();
    QDBusConnection dbus_connection =  QDBusConnection::connectToBus(dbus_address, dbus_name);
    if (dbus_connection.isConnected()) {
        qDebug() << "DBus connection established";
    } else {
        qWarning() << "DBus connection failed";
    }
    return dbus_connection;
}

}
