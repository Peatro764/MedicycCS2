#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>
#include "VirtualNSingle.h"
#include "VirtualNSingleContainer.h"

#include "Parameters.h"
#include "DBus.h"
#include "NSingleRepo.h"
#include "MultiplexedNSingle.h"
#include "Controller.h"
#include "nsingle_adaptor.h"
#include "nsinglecontroller_adaptor.h"
#include <QtWidgets/QApplication>
#include <QtDBus/QDBusConnection>

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

// Connect commands coming from either the controller or the dbus to the multiplexednsingle
void ConnectSignalsBetweenVirtualAndMultiplexed(hw_nsingle::VirtualNSingle *v_nsingle, hw_nsingle::MultiplexedNSingle *m_nsingle) {
    if (v_nsingle->config().multichannel()) {
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOn, m_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::On));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOff, m_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::Off));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReset, m_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::Reset));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadState, m_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::ReadState));
    } else {
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOn, m_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::On));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOff, m_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::Off));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReset, m_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::Reset));
        QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadState, m_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::ReadState));
    }
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CheckConnection, m_nsingle, &hw_nsingle::MultiplexedNSingle::IsConnected);
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadAddress, m_nsingle, &hw_nsingle::MultiplexedNSingle::ReadAddress);
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandPing, m_nsingle, &hw_nsingle::MultiplexedNSingle::Ping);
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadAddressSetPoint, m_nsingle, &hw_nsingle::MultiplexedNSingle::ReadAddressSetPoint);
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1SetPoint, m_nsingle,&hw_nsingle::MultiplexedNSingle::ReadChannel1SetPoint);
    QObject::connect(v_nsingle, qOverload<QString>(&hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1), m_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::ReadChannel1));
    QObject::connect(v_nsingle, qOverload<QString, int>(&hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1), m_nsingle, qOverload<QString, int>(&hw_nsingle::MultiplexedNSingle::ReadChannel1));
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteChannel1SetPoint, m_nsingle, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::WriteChannel1SetPoint));
    QObject::connect(v_nsingle, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteAndVerifyChannel1SetPoint, m_nsingle, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::WriteAndVerifyChannel1SetPoint));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./MultiplexedNSingleDBus \"Multiplexed_1\"";
        return -1;
    }

    QString name = QCoreApplication::arguments().at(1);
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
        qDebug() << "Creating MultiplexedNSingle";
        hw_nsingle::NSingleConfig nsingle_config = repo.GetNSingleConfig(name);
        hw_nsingle::MultiplexConfig multiplex_config = repo.GetMultiplexConfig(name);
        hw_nsingle::MultiplexedNSingle multiplex_nsingle(nsingle_config, multiplex_config);

        hw_nsingle::NSingleDbusAddress dbus_address =  repo.GetNSingleDbusAddress(name);
        new NSingleInterfaceAdaptor(&multiplex_nsingle);
        if (dbus_connection.registerObject(dbus_address.nsingle_object_name(), &multiplex_nsingle)) {
            qDebug() << "Successfully registered multiplexed nsingle object on dbus";
        } else {
            qWarning() << "Failed registering multiplexed nsingle object on dbus: " << dbus_connection.lastError().message();
        }

        qDebug() << "Creating all virtual nsingles and their associated controllers";
        hw_nsingle::VirtualNSingleContainer v_nsingles_container;;

        QMap<QString, hw_nsingle::Controller*> controllers;
        int delay(900);
        int cnt(0);
        for (auto& ch : multiplex_config.channels()) {
            qDebug() << "Creating virtual nsingle " << ch;
            hw_nsingle::NSingleConfig ch_config = repo.GetNSingleConfig(ch);
            hw_nsingle::VirtualNSingle *v_nsingle = new hw_nsingle::VirtualNSingle(ch_config);
            ConnectSignalsBetweenVirtualAndMultiplexed(v_nsingle, &multiplex_nsingle);
            v_nsingles_container.Add(ch, v_nsingle);

            hw_nsingle::Controller *controller = new hw_nsingle::Controller(v_nsingle, delay * cnt++);
            controllers[ch] = controller;

            hw_nsingle::NSingleDbusAddress ch_dbus_address =  repo.GetNSingleDbusAddress(ch);
            new NSingleInterfaceAdaptor(v_nsingle);
            if (dbus_connection.registerObject(ch_dbus_address.nsingle_object_name(), v_nsingle)) {
                qDebug() << "Successfully registered nsingle object on dbus";
            } else {
                qWarning() << "Failed registering nsingle object on dbus: " << dbus_connection.lastError().message();
            }
            new NSingleControllerInterfaceAdaptor(controller);
            if (dbus_connection.registerObject(ch_dbus_address.controller_object_name(), controller)) {
                qDebug() << "Successfully registered nsingle controller object on dbus";
            } else {
                qWarning() << "Failed registering nsingle controller object on dbus: " << dbus_connection.lastError().message();
            }
        } // for channels

        if (dbus_connection.registerService(dbus_address.interface_address())) {
            qDebug() << "Successfully registered dbus service";
        } else {
            qWarning() << "Failed registering dbus service: " << dbus_connection.lastError().message();
        }

        // Connect all signals from the multiplexnsingle that should be relayed to the correct virtualnsingle (and then onto the respective controller and dbus)
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_Connected, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::Connected);
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_Disconnected, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::Disconnected);
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_IOLoad, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::IOLoad);
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_Local, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::Local);
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_Remote, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::Remote);
        QObject::connect(&multiplex_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::SIGNAL_On), &v_nsingles_container, qOverload<>(&hw_nsingle::VirtualNSingleContainer::On));
        QObject::connect(&multiplex_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Off), &v_nsingles_container, qOverload<>(&hw_nsingle::VirtualNSingleContainer::Off));
        QObject::connect(&multiplex_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Error), &v_nsingles_container, qOverload<>(&hw_nsingle::VirtualNSingleContainer::Error));
        QObject::connect(&multiplex_nsingle, qOverload<>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Ok), &v_nsingles_container, qOverload<>(&hw_nsingle::VirtualNSingleContainer::Ok));
        QObject::connect(&multiplex_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::SIGNAL_On), &v_nsingles_container, qOverload<QString>(&hw_nsingle::VirtualNSingleContainer::On));
        QObject::connect(&multiplex_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Off), &v_nsingles_container, qOverload<QString>(&hw_nsingle::VirtualNSingleContainer::Off));
        QObject::connect(&multiplex_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Error), &v_nsingles_container, qOverload<QString>(&hw_nsingle::VirtualNSingleContainer::Error));
        QObject::connect(&multiplex_nsingle, qOverload<QString>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Ok), &v_nsingles_container, qOverload<QString>(&hw_nsingle::VirtualNSingleContainer::Ok));
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_AddressValue, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::AddressValue);
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_AddressSetPoint, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::AddressSetPoint);
        QObject::connect(&multiplex_nsingle, qOverload<QString, double, bool>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1Value), &v_nsingles_container, qOverload<QString, double, bool>(&hw_nsingle::VirtualNSingleContainer::Channel1Value));
        QObject::connect(&multiplex_nsingle, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1Value), &v_nsingles_container, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::VirtualNSingleContainer::Channel1Value));
        QObject::connect(&multiplex_nsingle, qOverload<QString, double, bool>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1SetPoint), &v_nsingles_container, qOverload<QString, double, bool>(&hw_nsingle::VirtualNSingleContainer::Channel1SetPoint));
        QObject::connect(&multiplex_nsingle, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1SetPoint), &v_nsingles_container, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::VirtualNSingleContainer::Channel1SetPoint));
        QObject::connect(&multiplex_nsingle, &hw_nsingle::MultiplexedNSingle::SIGNAL_IOError, &v_nsingles_container, &hw_nsingle::VirtualNSingleContainer::IOError);

        qDebug() << "Application startup";
        return app.exec();
    } catch (std::exception& exc) {
        qWarning() << "Caught exception: " << exc.what() << "\nThe program will terminate";
        return -1;
    }

    return app.exec();
}

