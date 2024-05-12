#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QtDBus/QDBusConnection>

#include "VirtualOmron.h"
#include "OmronSim.h"
#include "omron_adaptor.h"
#include "DBus.h"
#include "Parameters.h"

namespace omron = medicyc::cyclotroncontrolsystem::simulators::omron;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    omron::VirtualOmron omron;
    new OmronInterfaceAdaptor(&omron);

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    if (dbus_connection.registerObject("/Omron", &omron)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
    }
    if (dbus_connection.registerService("medicyc.cyclotron.hardware.omron.source")) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
    }

    omron::OmronSim sim(&omron);
    QObject::connect(&omron, &omron::VirtualOmron::SIGNAL_ReadRequest, &sim, &omron::OmronSim::ReadRequest);
    QObject::connect(&sim, &omron::OmronSim::SIGNAL_ReplyToReadRequest, &omron, &omron::VirtualOmron::SIGNAL_BitRead);
    QObject::connect(&omron, &omron::VirtualOmron::SIGNAL_WriteRequest, &sim, &omron::OmronSim::WriteRequest);
    QObject::connect(&sim, &omron::OmronSim::SIGNAL_ReplyToWriteRequest, &omron, &omron::VirtualOmron::SIGNAL_BitWritten);

    sim.show();
    return app.exec();
}


