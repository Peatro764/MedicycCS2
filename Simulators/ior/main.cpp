#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "IORSim.h"
#include "VirtualIOR.h"
#include "ior_adaptor.h"
#include "DBus.h"
#include "Parameters.h"

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;
namespace sim_ior = medicyc::cyclotroncontrolsystem::simulators::ior;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    sim_ior::VirtualIOR ior;
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

    sim_ior::IORSim sim(&ior);

    QObject::connect(&ior, &sim_ior::VirtualIOR::SIGNAL_ReadValueRequest, &sim, &sim_ior::IORSim::ReadValueRequest);
    QObject::connect(&ior, &sim_ior::VirtualIOR::SIGNAL_ReadBlockRequest, &sim, &sim_ior::IORSim::ReadMaskRequest);
    QObject::connect(&sim, &sim_ior::IORSim::SIGNAL_ReplyToReadValueRequest, &ior, &sim_ior::VirtualIOR::SIGNAL_ReceivedChannelValue);
    QObject::connect(&sim, &sim_ior::IORSim::SIGNAL_ReplyToReadMaskRequest, &ior, &sim_ior::VirtualIOR::SIGNAL_ReceivedChannelMask);

    sim.show();
    return app.exec();
}


