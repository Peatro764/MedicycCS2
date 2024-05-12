#include "BeamLineState.h"

#include <QStandardPaths>
#include <QSignalTransition>
#include <QDateTime>
#include <QTimer>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::beamlinestate {

namespace global = medicyc::cyclotroncontrolsystem::global;

BeamLineState::BeamLineState() :
    omron_interface_("medicyc.cyclotron.hardware.omron.vdf", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    ior_interface_("medicyc.cyclotron.hardware.ior", "/IOR", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    m1_interface_("medicyc.cyclotron.nsingle.Dipole_M1", "/Controller", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    m2_interface_("medicyc.cyclotron.nsingle.Dipole_M2", "/Controller", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    m4_interface_("medicyc.cyclotron.nsingle.Dipole_M4", "/Controller", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    qDebug() << "BeamLineState::BeamLineState";

    SetupOmron();
    SetupIOR();
    SetupNSingles();
}

BeamLineState::~BeamLineState()
{
}

void BeamLineState::SetupOmron() {
    if (!omron_interface_.isValid()) {
        qWarning() << "BeamLineState::SetupOmron OMRON interface not valid";
    }
    QTimer *timer = new QTimer;
    timer->setInterval(OMRON_READINTERVAL);
    timer->setSingleShot(false);
    timer->start();
    QObject::connect(timer, &QTimer::timeout, this, [&]() {
        omron_interface_.ReadChannel("Q CF9-2 OUT");
        omron_interface_.ReadChannel("Q QZ10 OUT");
        omron_interface_.ReadChannel("Q CF10 OUT");
        omron_interface_.ReadChannel("Q Guillotine Ouverte");
        omron_interface_.ReadChannel("Q Diaphragme Proton");
    });
    QObject::connect(&omron_interface_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&](const QString& channel, bool state) {
        // Logic omron: CFX OUT = 0 -> inserted. Thus give argument !state
        if (channel == "Q CF9-2 OUT") emit SIGNAL_CFState("CF9-2", !state);
        if (channel == "Q CF10 OUT") emit SIGNAL_CFState("CF10", !state);
        if (channel == "Q QZ10 OUT") emit SIGNAL_QZState("QZ10", !state);
        if (channel == "Q Guillotine Ouverte") emit SIGNAL_GuillotineState(state);
        if (channel == "Q Diaphragme Proton") emit SIGNAL_DiaphragmeState(state);
    });
}

void BeamLineState::SetupIOR() {
    if (!ior_interface_.isValid()) {
        qWarning() << "BeamLineState::SetupIOR IOR interface not valid";
    }
    QObject::connect(&ior_interface_, &medicyc::cyclotron::IORInterface::SIGNAL_ReceivedChannelValue, this, [&](const QString& channel, bool state) {
        // Logic IOR: 0 = CF, QZ, DT = inserted, thus give !state to SetXState
        if (channel.startsWith("CF")) emit SIGNAL_CFState(channel, !state);
        if (channel.startsWith("QZ")) emit SIGNAL_QZState(channel, !state);
        if (channel.startsWith("DT")) emit SIGNAL_DTState(state);
    });
}

void BeamLineState::SetupNSingles() {
    QObject::connect(&m1_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [&]() { emit SIGNAL_DipoleState("M1", true); });
    QObject::connect(&m1_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [&]() { emit SIGNAL_DipoleState("M1", false); });
    QObject::connect(&m2_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [&]() { emit SIGNAL_DipoleState("M2", true); });
    QObject::connect(&m2_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [&]() { emit SIGNAL_DipoleState("M2", false); });
    QObject::connect(&m4_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [&]() { emit SIGNAL_DipoleState("M4", true); });
    QObject::connect(&m4_interface_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [&]() { emit SIGNAL_DipoleState("M4", false); });
}

}
