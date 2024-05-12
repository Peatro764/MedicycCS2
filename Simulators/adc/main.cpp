#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "ADCSim.h"
#include "VirtualADC.h"
#include "adc_adaptor.h"
#include "DBus.h"
#include "Parameters.h"

namespace simu_adc = medicyc::cyclotroncontrolsystem::simulators::adc;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    simu_adc::VirtualADC adc;
    new ADCInterfaceAdaptor(&adc);

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    if (dbus_connection.registerObject("/ADC", &adc)) {
        qDebug() << "Successfully registered object on dbus";
    } else {
        qWarning() << "Failed registering object on dbus: " << dbus_connection.lastError().message();
    }
    if (dbus_connection.registerService("medicyc.cyclotron.hardware.adc")) {
        qDebug() << "Successfully registered dbus service";
    } else {
        qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
    }
    simu_adc::ADCSim sim(&adc);

    QObject::connect(&adc, &simu_adc::VirtualADC::SIGNAL_ReadRequest, &sim, &simu_adc::ADCSim::ReadRequest);
    QObject::connect(&sim, &simu_adc::ADCSim::SIGNAL_ReplyToReadRequest, &adc, &simu_adc::VirtualADC::SIGNAL_ReceivedChannelValue);

    sim.show();
    return app.exec();
}


