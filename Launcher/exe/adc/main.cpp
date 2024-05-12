#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

#include "Parameters.h"
#include "DBus.h"

#include "ADCRepo.h"
#include "ADC.h"
#include "adc_adaptor.h"

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Setting up ADC";

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
    hw_adc::ADCRepo repo("_prod");
    if (!repo.Connect(5)) {
        qWarning() << "Connection to db failed, bailing out";
        return -1;
    }

    std::vector<hw_adc::Channel> channels;
    QStringList names;

    try {
        names = repo.GetADCChannelNames();

        for (QString& name : names) {
            channels.push_back(repo.GetADCChannel(name));
            qDebug() << "Found channel: " << name;
        }

        qDebug() << "Create ADC";
        hw_adc::ADC adc;
        adc.SetChannels(channels);

        new ADCInterfaceAdaptor(&adc);
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

        qDebug() << "Application startup";
        return app.exec();
    } catch (std::exception& exc) {
        qWarning() << "Caught exception: " << exc.what() << "\nThe program will terminate";
        return -1;
    }
}
