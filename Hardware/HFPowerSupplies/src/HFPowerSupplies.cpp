#include "HFPowerSupplies.h"

#include <QStandardPaths>
#include <QSettings>
#include <QDateTime>
#include <QDebug>

#include "TimedState.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

HFPowerSupplies::HFPowerSupplies() :
    adc_("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    omron_("medicyc.cyclotron.hardware.omron.hf", "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    SetupTopLevelDevices();
    SetupBottomLevelDevices();
    QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead , this, &HFPowerSupplies::InterpretOmronBitRead);
    SetupStateMachine();

    QTimer *tTopLevelDevices = new QTimer(this);
    tTopLevelDevices->setInterval(4000);
    tTopLevelDevices->setSingleShot(false);
    tTopLevelDevices->start();
    QObject::connect(tTopLevelDevices, &QTimer::timeout, this, &HFPowerSupplies::ReadTopLevelDeviceStatus);

    QTimer *tDbus = new QTimer(this);
    tDbus->setInterval(3000);
    tDbus->setSingleShot(false);
    tDbus->start();
    QObject::connect(tDbus, &QTimer::timeout, this, &HFPowerSupplies::CheckDbus);

    QTimer *tAlstom = new QTimer(this);
    tAlstom->setInterval(20000);
    tAlstom->setSingleShot(false);
    tAlstom->start();
    QObject::connect(tAlstom, &QTimer::timeout, this, &HFPowerSupplies::ReadAlstom);

    qDebug() << "Omron " << omron_.isValid();
    qDebug() << "ADC " << adc_.isValid();
    qDebug() << "Logger " << logger_.isValid();

    ReadTopLevelDeviceStatus();
    ReadBottomLevelDeviceStatus();
}

HFPowerSupplies::~HFPowerSupplies() {

}

void HFPowerSupplies::Startup() {
    emit SIGNAL_Startup();
}

void HFPowerSupplies::Shutdown() {
    emit SIGNAL_Shutdown();
}

void HFPowerSupplies::Interrupt() {
    emit SIGNAL_Interrupt();
}

// TODO make these impulses

void HFPowerSupplies::PreAmplisOn() {
    omron_.WriteChannel(CMD_COMMANDPREAMPLISON, true);
    QTimer::singleShot(PULSE_WIDTH, this, [&]() { omron_.WriteChannel(CMD_COMMANDPREAMPLISON, false); });
}

void HFPowerSupplies::PreAmplisOff() {
    omron_.WriteChannel(CMD_COMMANDPREAMPLISOFF, true);
    QTimer::singleShot(PULSE_WIDTH, this, [&]() { omron_.WriteChannel(CMD_COMMANDPREAMPLISOFF, false); });
}

void HFPowerSupplies::AmplisOn() {
    omron_.WriteChannel(CMD_COMMANDAMPLISON, true);
    QTimer::singleShot(PULSE_WIDTH, this, [&]() { omron_.WriteChannel(CMD_COMMANDAMPLISON, false); });
}

void HFPowerSupplies::AmplisOff() {
    omron_.WriteChannel(CMD_COMMANDAMPLISOFF, true);
    QTimer::singleShot(PULSE_WIDTH, this, [&]() { omron_.WriteChannel(CMD_COMMANDAMPLISOFF, false); });
}

void HFPowerSupplies::ReadAlstom() {
    adc_.Read("V.Alstom");
    adc_.Read("I.Alstom");
}

void HFPowerSupplies::ReadBottomLevelDeviceStatus() {
    qDebug() << QDateTime::currentDateTime() << " HFPowerSupplies::ReadBottomLevelDeviceStatus " << bottomlevel_devices_.size();
    for (auto name : bottomlevel_devices_.keys()) {
        Device *d = bottomlevel_devices_.value(name);
        omron_.ReadChannel(d->GetCmdIsOn());
        omron_.ReadChannel(d->GetCmdDefautActive());
        if (!d->GetCmdVoltage().isEmpty()) adc_.Read(d->GetCmdVoltage());
    }
}

void HFPowerSupplies::ReadTopLevelDeviceStatus() {
    qDebug() << QDateTime::currentDateTime() <<  " HFPowerSupplies::ReadTopLevelDeviceStatus " << toplevel_devices_.size();
    for (auto name : toplevel_devices_.keys()) {
        Device *d = toplevel_devices_.value(name);
        omron_.ReadChannel(d->GetCmdIsOn());
        omron_.ReadChannel(d->GetCmdIsOff());
        omron_.ReadChannel(d->GetCmdDefautActive());
    }
}

void HFPowerSupplies::SetupTopLevelDevices() {
    toplevel_devices_["HF1 PreAmplis"] = new Device("HF1 PreAmplis", "Q HF1 PreAmplis On", "Q HF1 PreAmplis Off", "Q HF1 PreAmplis Defaut", "");
    toplevel_devices_["HF2 PreAmplis"] = new Device("HF2 PreAmplis", "Q HF2 PreAmplis On", "Q HF2 PreAmplis Off", "Q HF2 PreAmplis Defaut", "");
    toplevel_devices_["HF1 Amplis"] = new Device("HF1 Amplis", "Q HF1-HF2 Amplis On", "Q HF1 Amplis Off", "Q HF1 Alims Ampli Defaut", "");
    toplevel_devices_["HF2 Amplis"] = new Device("HF2 Amplis", "Q HF1-HF2 Amplis On", "Q HF2 Amplis Off", "Q HF2 Alims Ampli Defaut", "");

    for (auto name : toplevel_devices_.keys()) {
        Device *d = toplevel_devices_.value(name);

        QObject::connect(d, &Device::SIGNAL_Defaut_Active, this, [&, name, this]() { emit SIGNAL_Device_Defaut(name, true); });
        QObject::connect(d, &Device::SIGNAL_No_Defaut, this, [&, name, this]() { emit SIGNAL_Device_Defaut(name, false); });
        QObject::connect(d, &Device::SIGNAL_Off, this, [&, name, this]() { emit SIGNAL_Device_State(name, false); });
        QObject::connect(d, &Device::SIGNAL_On, this, [&, name, this]() { emit SIGNAL_Device_State(name, true); });

        QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&, d](const QString& channel, bool content) {
            if (channel == d->GetCmdIsOn()) d->SetOn(content);
        });
        QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&, d](const QString& channel, bool content) {
            if (channel == d->GetCmdIsOff()) d->SetOff(content);
        });
        QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&, d, this](const QString& channel, bool content) {
            if (channel == d->GetCmdDefautActive()) {
                if (!d->IsDefaut() && content) {
                    this->WarningMessage(channel);
                }
                d->SetDefaut(content);
            }
        });
    }
}

void HFPowerSupplies::SetupBottomLevelDevices() {
    bottomlevel_devices_["HF1 Grille PreAmpli"] = new Device("HF1 Grille PreAmpli", "Q HF1 Alim Grille PreAmpli On", "", "Q HF1 Alim Grille PreAmpli Defaut", "V.Grille Preamp 1");
    bottomlevel_devices_["HF1 Anode PreAmpli"] = new Device("HF1 Anode PreAmpli", "Q HF1 Anode PreAmpli On", "", "Q HF1 Anode PreAmpli Defaut", "V.Anode Preamp 1");
    bottomlevel_devices_["HF1 Ecran PreAmpli"] = new Device("HF1 Ecran PreAmpli", "Q HF1 Ecran PreAmpli On", "", "Q HF1 Ecran PreAmpli Defaut", "V.Ecran Preamp 1");
    bottomlevel_devices_["HF1 Grille Ampli"] = new Device("HF1 Grille Ampli", "Q HF1 Alim Grille Ampli On", "", "Q HF1 Alim Grille Ampli Defaut", "V.Grille Ampli 1");
    bottomlevel_devices_["HF1 Ecran Ampli"] = new Device("HF1 Ecran Ampli", "Q HF1 Alim Ecran Ampli On", "", "Q HF1 Alim Ecran Ampli Defaut", "V.Ecran Ampli 1");
    bottomlevel_devices_["HF1 Filament PreAmpli"] = new Device("HF1 Filament PreAmpli", "Q HF1 Filament PreAmpli On", "", "Q HF1 Filament PreAmpli Defaut", "");
    bottomlevel_devices_["HF1 Filament Ampli"] = new Device("HF1 Filament Ampli", "Q HF1 Filament Ampli On", "", "Q HF1 Filament Ampli Defaut", "");

    bottomlevel_devices_["HF2 Grille PreAmpli"] = new Device("HF2 Grille PreAmpli", "Q HF2 Alim Grille PreAmpli On", "", "Q HF2 Alim Grille PreAmpli Defaut", "V.Grille Preamp 2");
    bottomlevel_devices_["HF2 Anode PreAmpli"] = new Device("HF2 Anode PreAmpli", "Q HF2 Anode PreAmpli On", "", "Q HF2 Anode PreAmpli Defaut", "V.Anode Preamp 2");
    bottomlevel_devices_["HF2 Ecran PreAmpli"] = new Device("HF2 Ecran PreAmpli", "Q HF2 Ecran PreAmpli On", "", "Q HF2 Ecran PreAmpli Defaut", "V.Ecran Preamp 2");
    bottomlevel_devices_["HF2 Grille Ampli"] = new Device("HF2 Grille Ampli", "Q HF2 Alim Grille Ampli On", "", "Q HF2 Alim Grille Ampli Defaut", "V.Grille Ampli 2");
    bottomlevel_devices_["HF2 Ecran Ampli"] = new Device("HF2 Ecran Ampli", "Q HF2 Alim Ecran Ampli On", "", "Q HF2 Alim Ecran Ampli Defaut", "V.Ecran Ampli 2");
    bottomlevel_devices_["HF2 Filament PreAmpli"] = new Device("HF2 Filament PreAmpli", "Q HF2 Filament PreAmpli On", "", "Q HF2 Filament PreAmpli Defaut", "");
    bottomlevel_devices_["HF2 Filament Ampli"] = new Device("HF2 Filament Ampli", "Q HF2 Filament Ampli On", "", "Q HF2 Filament Ampli Defaut", "");

    bottomlevel_devices_["Alim THT"] = new Device("Alim THT", "Q Alim THT On", "", "Q Alim THT Defaut", "");
    bottomlevel_devices_["Alim THT Aux"] = new Device("Alim THT Aux", "Q Alim THT Aux On", "", "Q Alim THT Aux Defaut", "");

    for (auto name : bottomlevel_devices_.keys()) {
        Device *d = bottomlevel_devices_.value(name);

        QObject::connect(d, &Device::SIGNAL_Defaut_Active, this, [&, name, this]() { emit SIGNAL_Device_Defaut(name, true); });
        QObject::connect(d, &Device::SIGNAL_No_Defaut, this, [&, name, this]() { emit SIGNAL_Device_Defaut(name, false); });
        QObject::connect(d, &Device::SIGNAL_Off, this, [&, name, this]() { emit SIGNAL_Device_State(name, false); });
        QObject::connect(d, &Device::SIGNAL_On, this, [&, name, this]() { emit SIGNAL_Device_State(name, true); });
        QObject::connect(d, &Device::SIGNAL_Voltage, this, [&, name, this](double value) { emit SIGNAL_Device_Voltage(name, value); });

        QObject::connect(&adc_, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue, this, [&, d](const QString& channel, double value) {
            if (channel == d->GetCmdVoltage()) {
                d->SetVoltage(value);
            }
        });
        QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&, d](const QString& channel, bool content) {
            if (channel == d->GetCmdIsOn()) {
                d->SetOn(content);
                d->SetOff(!content);
            }
        });
        QObject::connect(&omron_, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [&, d, this](const QString& channel, bool content) {
            if (channel == d->GetCmdDefautActive()) {
                if (!d->IsDefaut() && content) {
                    this->WarningMessage(channel);
                }
                d->SetDefaut(content);
            }
        });
    }
}

void HFPowerSupplies::SetupStateMachine() {
    qDebug() << "HFPowerSupplies::SetupStateMachine";
    QState *sSuperState = new QState();

    QState *sDisconnected = new QState(sSuperState);
    QState *sConnected = new QState(sSuperState);

        QState *sError = new QState(sConnected);
        QState *sIdle = new QState(sConnected);
        QState *sDefaut = new QState(sConnected);
        QState *sAllOn = new QState(sConnected);
        QState *sAllOff = new QState(sConnected);

        global::TimedState *sStartup = new global::TimedState(sConnected, 300000, "Délai d'attende de la séquence de démarrage des alimentations", sIdle);
            global::TimedState *sVerifyStateBeforeStartup = new global::TimedState(sStartup, 7000, "Echec de la vérification de l'état, démarrage des alimentations arrêté.", sIdle);
            global::TimedState *sWriteStartAll = new global::TimedState(sStartup, 6000, "Nombre maximum d'essais de démarrage des alimentations atteint", sIdle);
                global::TimedState *sWriteStartAll_FlankUp = new global::TimedState(sWriteStartAll, 1000, "Réessayer la commande de démarrage des alimentations flanc montant");
                global::TimedState *sWriteStartAll_FlankDown = new global::TimedState(sWriteStartAll, 1000, "Réessayer la commande de démarrage des alimentations flanc descendent");
            global::TimedState *sVerifyStartupOngoing = new global::TimedState(sStartup, 5000, "L'automate ne signale pas que le démarrage des alimentations est en cours", sIdle);
            global::TimedState *sWaitUntilAllStarted = new global::TimedState(sStartup, 240000, "Délai d'attente pour le démarrage des alimentations", sIdle);
            global::TimedState *sVerifyAllStarted = new global::TimedState(sStartup, 10000, "Echec de la vérification de l'état de toutes les alimentations", sIdle);

        global::TimedState *sShutdown = new global::TimedState(sConnected, 300000, "Délai d'attende de la séquence de shutdown des alimentations", sIdle);
            global::TimedState *sVerifyStateBeforeShutdown = new global::TimedState(sShutdown, 7000, "Echec de la vérification de l'état, shutdown des alimentations arrêté.", sIdle);
            global::TimedState *sWriteStopAll = new global::TimedState(sShutdown, 6000, "Nombre maximum d'essais de shutdown des alimentations atteint", sIdle);
                global::TimedState *sWriteStopAll_FlankUp = new global::TimedState(sWriteStopAll, 1000, "Réessayer la commande de shutdown des alimentations flanc montant");
                global::TimedState *sWriteStopAll_FlankDown = new global::TimedState(sWriteStopAll, 1000, "Réessayer la commande de Shutdown des alimentations flanc descendent");
            global::TimedState *sWaitUntilAllStopped = new global::TimedState(sShutdown, 120000, "Délai d'attente pour le shutdown des alimentations", sIdle);

        // SuperState
        sSuperState->setInitialState(sDisconnected);

        // Disconnected
        auto tDbusOk = sDisconnected->addTransition(this, &HFPowerSupplies::SIGNAL_DbusOk, sConnected);
        QObject::connect(tDbusOk, &QSignalTransition::triggered, this, [&]() { InfoMessage("Dbus connecté"); });

        // Connected
        sConnected->setInitialState(sIdle);
        auto tInterrupt = sConnected->addTransition(this, &HFPowerSupplies::SIGNAL_Interrupt, sIdle);
        QObject::connect(tInterrupt, &QSignalTransition::triggered, this, [&]() { WarningMessage("Les alimentations HF a été interrompu"); });
        auto tDbusError = sConnected->addTransition(this, &HFPowerSupplies::SIGNAL_DbusError, sDisconnected);
        QObject::connect(tDbusError, &QSignalTransition::triggered, this, [&]() {
            ErrorMessage("Dbus déconnecté");
            emit SIGNAL_Error();
        });
        sConnected->addTransition(this, &HFPowerSupplies::SIGNAL_Startup, sStartup);
        sConnected->addTransition(this, &HFPowerSupplies::SIGNAL_Shutdown, sShutdown);
        auto tDefaut = sConnected->addTransition(this, &HFPowerSupplies::SIGNAL_Defaut_Active, sDefaut);
        QObject::connect(tDefaut, &QSignalTransition::triggered, this, [&]() {
            ErrorMessage("Une alimentation a signalé un défaut");
            emit SIGNAL_Error();
        });
        QTimer *sTickTimer = new QTimer(sConnected);
        sTickTimer->setInterval(1500);
        sTickTimer->setSingleShot(false);
        sTickTimer->start();
        QObject::connect(sConnected, &QState::entered, sTickTimer, qOverload<>(&QTimer::start));
        QObject::connect(sConnected, &QState::exited, sTickTimer, &QTimer::stop);
        QObject::connect(sTickTimer, &QTimer::timeout, this, &HFPowerSupplies::Ping);

        // Error
        QObject::connect(sError, &QState::entered, this, &HFPowerSupplies::SIGNAL_Error);
        sError->addTransition(sError, &QState::entered, sIdle);

        // Idle
        auto tAllOn = sIdle->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_True, sAllOn);
        QObject::connect(tAllOn, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations sont démarrés"); });
        auto tAllOff = sIdle->addTransition(this, &HFPowerSupplies::SIGNAL_AllOff_True, sAllOff);
        QObject::connect(tAllOff, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations sont coupés"); });

        // Defaut
        sDefaut->addTransition(new QSignalTransition(this, &HFPowerSupplies::SIGNAL_Defaut_Active)); // Swallow
        sDefaut->addTransition(this, &HFPowerSupplies::SIGNAL_Defaut_NonActive, sIdle);

        // AllOn
        auto tAllNotOnInAllOn = sAllOn->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_False, sError);
        QObject::connect(tAllNotOnInAllOn, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Au moins une alimentation est coupé"); });

        // AllOff
        auto tAllNotOffInAllOff = sAllOff->addTransition(this, &HFPowerSupplies::SIGNAL_AllOff_False, sIdle);
        QObject::connect(tAllNotOffInAllOff, &QSignalTransition::triggered, this, [&]() { InfoMessage("Au moins une alimentation est allumée"); });

        // Startup
        sStartup->setInitialState(sVerifyStateBeforeStartup);
        QObject::connect(sStartup, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);

            // VerifyStateBeforeStartup
            QObject::connect(sVerifyStateBeforeStartup, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
            auto tAllNotOnInStartup = sVerifyStateBeforeStartup->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_False, sWriteStartAll);
            QObject::connect(tAllNotOnInStartup, &QSignalTransition::triggered, this, [&]() { InfoMessage("Au moins une alimentation non démarrée, procédure de démarrage lancée"); });
            auto tAllOnInStartup = sVerifyStateBeforeStartup->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_True, sAllOn);
            QObject::connect(tAllOnInStartup, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations sont déjà démarré, procédure de démarrage interrompue"); });

            // WriteStartAll
            QObject::connect(sWriteStartAll, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
            sWriteStartAll->setInitialState(sWriteStartAll_FlankUp);
            sWriteStartAll->addTransition(new QSignalTransition(this, &HFPowerSupplies::SIGNAL_Defaut_Active)); // Swallow
            sWriteStartAll->addTransition(new QSignalTransition(this, &HFPowerSupplies::SIGNAL_Interrupt)); // Swallow

                // WriteStartAll_FlankUp
                QObject::connect(sWriteStartAll_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::WarningMessage);
                QObject::connect(sWriteStartAll_FlankUp, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CMD_COMMANDALLON, true);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CMD_COMMANDALLON); });
                });
                sWriteStartAll_FlankUp->addTransition(this, &HFPowerSupplies::SIGNAL_Startup_Commanded_True, sWriteStartAll_FlankDown);

                // WriteStartAll_FlankDown
                QObject::connect(sWriteStartAll_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::WarningMessage);
                QObject::connect(sWriteStartAll_FlankDown, &QState::entered, this, [&]() {
                    omron_.WriteChannel(CMD_COMMANDALLON, false);
                    QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CMD_COMMANDALLON); });
                });
                auto tWriteStartAll_Ok = sWriteStartAll_FlankDown->addTransition(this, &HFPowerSupplies::SIGNAL_Startup_Commanded_False, sVerifyStartupOngoing);
                QObject::connect(tWriteStartAll_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion démarrage des alimentations écrite"); });

            // VerifyStartupOngoing
            QObject::connect(sVerifyStartupOngoing, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
            QTimer *tVerifyStartupOngoing = new QTimer(sVerifyStartupOngoing);
            tVerifyStartupOngoing->setInterval(1000);
            tVerifyStartupOngoing->setSingleShot(false);
            QObject::connect(sVerifyStartupOngoing, &QState::entered, tVerifyStartupOngoing, qOverload<>(&QTimer::start));
            QObject::connect(sVerifyStartupOngoing, &QState::exited, tVerifyStartupOngoing, &QTimer::stop);
            QObject::connect(tVerifyStartupOngoing, &QTimer::timeout, this, [&]() { omron_.ReadChannel(CMD_ISSTARTUPONGOING); });
            auto tStartupOngoing = sVerifyStartupOngoing->addTransition(this, &HFPowerSupplies::SIGNAL_StartUp_Ongoing_True, sWaitUntilAllStarted);
            QObject::connect(tStartupOngoing, &QSignalTransition::triggered, this, [&]() { DebugMessage("L'automate est en train de démarrer les alimentations"); });

            // WaitUntilAllStarted
            QObject::connect(sWaitUntilAllStarted, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
            QTimer *tWaitUntilAllStarted = new QTimer(sWaitUntilAllStarted);
            tWaitUntilAllStarted->setInterval(3000);
            tWaitUntilAllStarted->setSingleShot(false);
            QObject::connect(sWaitUntilAllStarted, &QState::entered, tWaitUntilAllStarted, qOverload<>(&QTimer::start));
            QObject::connect(sWaitUntilAllStarted, &QState::exited, tWaitUntilAllStarted, &QTimer::stop);
            QObject::connect(tWaitUntilAllStarted, &QTimer::timeout, this, [&]() { omron_.ReadChannel(CMD_ISSTARTUPONGOING); });
            auto tPLCFinished = sWaitUntilAllStarted->addTransition(this, &HFPowerSupplies::SIGNAL_StartUp_Ongoing_False, sVerifyAllStarted);
            QObject::connect(tPLCFinished, &QSignalTransition::triggered, this, [&]() { DebugMessage("L'automate a signalé qu'il a fini de démarrer les alimentations."); });

            // VerifyAllStarted
            QObject::connect(sVerifyAllStarted, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
            auto tVerifyAllStartedNotAllOn = sVerifyAllStarted->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_False, sError);
            QObject::connect(tVerifyAllStartedNotAllOn, &QSignalTransition::triggered, this, [&]() { ErrorMessage("Toutes les alimentations n'ont pas été démarrées"); });
            auto tVerifyAllStartedAllOn = sVerifyAllStarted->addTransition(this, &HFPowerSupplies::SIGNAL_AllOn_True, sAllOn);
            QObject::connect(tVerifyAllStartedAllOn, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations ont été vérifiées d'être démarrées"); });

            // Shutdown
            sShutdown->setInitialState(sVerifyStateBeforeShutdown);
            QObject::connect(sShutdown, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);

                // VerifyStateBeforeShutdown
                QObject::connect(sVerifyStateBeforeShutdown, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
                auto tAllNotOffInShutdown = sVerifyStateBeforeShutdown->addTransition(this, &HFPowerSupplies::SIGNAL_AllOff_False, sWriteStopAll);
                QObject::connect(tAllNotOffInShutdown, &QSignalTransition::triggered, this, [&]() { InfoMessage("Au moins une alimentation non coupé, procédure de shutdown lancée"); });
                auto tAllOffInShutdown = sVerifyStateBeforeShutdown->addTransition(this, &HFPowerSupplies::SIGNAL_AllOff_True, sAllOff);
                QObject::connect(tAllOffInShutdown, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations sont déjà coupés, procédure de shutdown interrompue"); });

                // WriteStopAll
                QObject::connect(sWriteStopAll, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
                sWriteStopAll->setInitialState(sWriteStopAll_FlankUp);
                sWriteStopAll->addTransition(new QSignalTransition(this, &HFPowerSupplies::SIGNAL_Defaut_Active)); // Swallow
                sWriteStopAll->addTransition(new QSignalTransition(this, &HFPowerSupplies::SIGNAL_Interrupt)); // Swallow

                    // WriteStopAll_FlankUp
                    QObject::connect(sWriteStopAll_FlankUp, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::WarningMessage);
                    QObject::connect(sWriteStopAll_FlankUp, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CMD_COMMANDALLOFF, true);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CMD_COMMANDALLOFF); });
                    });
                    sWriteStopAll_FlankUp->addTransition(this, &HFPowerSupplies::SIGNAL_Shutdown_Commanded_True, sWriteStopAll_FlankDown);

                    // WriteStopAll_FlankDown
                    QObject::connect(sWriteStopAll_FlankDown, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::WarningMessage);
                    QObject::connect(sWriteStopAll_FlankDown, &QState::entered, this, [&]() {
                        omron_.WriteChannel(CMD_COMMANDALLOFF, false);
                        QTimer::singleShot(PULSE_WIDTH, this, [this]() { omron_.ReadChannel(CMD_COMMANDALLOFF); });
                    });
                    auto tWriteStopAll_Ok = sWriteStopAll_FlankDown->addTransition(this, &HFPowerSupplies::SIGNAL_Shutdown_Commanded_False, sWaitUntilAllStopped);
                    QObject::connect(tWriteStopAll_Ok, &QSignalTransition::triggered, this, [&]() { DebugMessage("Commande d'impulsion shutdown des alimentations écrite"); });

               // WaitUntilAllStopped
               QObject::connect(sWaitUntilAllStopped, &QState::entered, this, [&]() { InfoMessage("Attend que toutes les alimentations soient coupées"); });
               QObject::connect(sWaitUntilAllStopped, &global::TimedState::SIGNAL_Timeout, this, &HFPowerSupplies::ErrorMessage);
               auto tWaitUntilAllStoppedAllStopped = sWaitUntilAllStopped->addTransition(this, &HFPowerSupplies::SIGNAL_AllOff_True, sAllOff);
               QObject::connect(tWaitUntilAllStoppedAllStopped, &QSignalTransition::triggered, this, [&]() { InfoMessage("Toutes les alimentations sont coupés"); });

        PrintStateChanges(sSuperState, "SuperState");
        PrintStateChanges(sConnected, "Connected");
        PrintStateChanges(sDisconnected, "Disconnected");
        PrintStateChanges(sIdle, "Idle");
        PrintStateChanges(sAllOn, "AllOn");
        PrintStateChanges(sAllOff, "AllOff");
        PrintStateChanges(sDefaut, "Defaut");
        PrintStateChanges(sStartup, "Startup");
        PrintStateChanges(sVerifyStateBeforeStartup, "VerifyStateBeforeStartup");
        PrintStateChanges(sWriteStartAll, "WriteStartAll");
        PrintStateChanges(sWriteStartAll_FlankUp, "sWriteStartAll_FlankUp");
        PrintStateChanges(sWriteStartAll_FlankDown, "sWriteStartAll_FlankDown");
        PrintStateChanges(sVerifyStartupOngoing, "sVerifyStartupOngoing");
        PrintStateChanges(sWaitUntilAllStarted, "WaitUntilAllStarted");
        PrintStateChanges(sVerifyAllStarted, "VerifyAllStarted");

        PrintStateChanges(sShutdown, "Shutdown");
        PrintStateChanges(sVerifyStateBeforeShutdown, "VerifyStateBeforeShutdown");
        PrintStateChanges(sWriteStopAll, "WriteStopAll");
        PrintStateChanges(sWriteStopAll_FlankUp, "sWriteStopAll_FlankUp");
        PrintStateChanges(sWriteStopAll_FlankDown, "sWriteStopAll_FlankDown");
        PrintStateChanges(sWaitUntilAllStopped, "WaitUntilAllStopped");

        sm_.addState(sSuperState);
        sm_.setInitialState(sSuperState);
        sm_.start();
}

void HFPowerSupplies::Ping() {
    qDebug() << QDateTime::currentDateTime() << " HFPowerSupplies::Ping";
    omron_.ReadChannel(CMD_ISALLON);
    omron_.ReadChannel(CMD_ISALLOFF);
    omron_.ReadChannel(CMD_ISDEFAUTACTIVE);
}

void HFPowerSupplies::PrintStateChanges(QAbstractState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << QDateTime::currentDateTime() << "HFPowerSupplies HF " << " -> " << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << QDateTime::currentDateTime() <<  "HFPowerSupplies HF " << " <- " << name; });
}

void HFPowerSupplies::InterpretOmronBitRead(const QString& channel, bool content) {
    if (channel == CMD_ISALLON) {
        content ? emit SIGNAL_AllOn_True() : emit SIGNAL_AllOn_False();
        if (content != all_on_) {
            ReadBottomLevelDeviceStatus();
        }
        all_on_ = content;
    } else if (channel == CMD_ISALLOFF) {
        content ? emit SIGNAL_AllOff_True() : emit SIGNAL_AllOff_False();
        if (content != all_off_) {
            ReadBottomLevelDeviceStatus();
        }
        all_off_ = content;
    } else if (channel == CMD_ISDEFAUTACTIVE) {
        content ? emit SIGNAL_Defaut_Active() : emit SIGNAL_Defaut_NonActive();
        if (content != defaut_active_) {
            ReadBottomLevelDeviceStatus();
        }
        defaut_active_ = content;
    } else if (channel == CMD_COMMANDALLON) {
        content ? emit SIGNAL_Startup_Commanded_True() : emit SIGNAL_Startup_Commanded_False();
    } else if (channel == CMD_COMMANDALLOFF) {
        content ? emit SIGNAL_Shutdown_Commanded_True() : emit SIGNAL_Shutdown_Commanded_False();
    } else if (channel == CMD_ISSTARTUPONGOING) {
        content ? emit SIGNAL_StartUp_Ongoing_True() : emit SIGNAL_StartUp_Ongoing_False();
    } else if (channel == CMD_ISSTARTUPFINISHED) {
        content ? emit SIGNAL_StartUp_Finished_True() : emit SIGNAL_StartUp_Finished_False();
    }
}

void HFPowerSupplies::CheckDbus() {
    if (omron_.isValid()) {
        if (!dbus_connected_) {
            dbus_connected_ = true;
            emit SIGNAL_DbusOk();
        }
    } else {
        if (dbus_connected_) {
            dbus_connected_ = false;
            emit SIGNAL_DbusError();
        }
    }
}

void HFPowerSupplies::ErrorMessage(QString message) {
    logger_.Error(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF", message);
}

void HFPowerSupplies::WarningMessage(QString message) {
    logger_.Warning(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF", message);
}

void HFPowerSupplies::InfoMessage(QString message) {
    logger_.Info(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF", message);
}

void HFPowerSupplies::DebugMessage(QString message) {
    logger_.Debug(QDateTime::currentDateTime().toMSecsSinceEpoch(), "HF", message);
}

} // namespace
