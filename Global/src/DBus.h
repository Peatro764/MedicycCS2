#ifndef MEDICYCCS2_GLOBAL_DBUS_H
#define MEDICYCCS2_GLOBAL_DBUS_H

#include <QtDBus/QDBusConnection>
#include <QString>
#include <QDebug>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::global {

QString GetDBusAddress(QString host, QString port);
QDBusConnection GetDBusConnection();

} // ns

#endif // DBUS_H
