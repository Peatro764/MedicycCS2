#ifndef HARDWARE_HFPOWERSUPPLIES_H
#define HARDWARE_HFPOWERSUPPLIES_H

#include <vector>
#include <QStateMachine>
#include <QTimer>

#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"
#include "Device.h"

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

class HFPowerSupplies : public QObject
{
    Q_OBJECT

public:
    explicit HFPowerSupplies();
    ~HFPowerSupplies();

public slots: // dbus
    void Startup();
    void Shutdown();
    void Interrupt();
    void PreAmplisOn();
    void PreAmplisOff();
    void AmplisOn();
    void AmplisOff();
    void ReadBottomLevelDeviceStatus();
    void ReadTopLevelDeviceStatus();
    void Ping();

signals: // Internal
    // Commands
    void SIGNAL_Startup();
    void SIGNAL_Shutdown();
    void SIGNAL_Interrupt();
    void SIGNAL_CmdPreAmplisOn();
    void SIGNAL_CmdPreAmplisOff();
    void SIGNAL_CmdAmplisOn();
    void SIGNAL_CmdAmplisOff();

    // Return of Startup/shutdown commands (read in the plc)
    void SIGNAL_Startup_Commanded_True();
    void SIGNAL_Startup_Commanded_False();
    void SIGNAL_Shutdown_Commanded_True();
    void SIGNAL_Shutdown_Commanded_False();
    void SIGNAL_StartUp_Ongoing_True();
    void SIGNAL_StartUp_Ongoing_False();
    // TODO: behaviour of below parameter not clear, might be removed
    void SIGNAL_StartUp_Finished_True();
    void SIGNAL_StartUp_Finished_False();

    void SIGNAL_AllOn_True();
    void SIGNAL_AllOn_False();
    void SIGNAL_AllOff_True();
    void SIGNAL_AllOff_False();
    void SIGNAL_Defaut_Active();
    void SIGNAL_Defaut_NonActive();

    void SIGNAL_DbusOk();
    void SIGNAL_DbusError();

signals: // External (Dbus)
    void SIGNAL_Error();
    void SIGNAL_Device_State(QString component, bool on);
    void SIGNAL_Device_Defaut(QString component, bool active);
    void SIGNAL_Device_Voltage(QString component, double value);

private slots:
    void InterpretOmronBitRead(const QString& channel, bool content);
    // Alstom current and voltage are never used in the control system
    // and thus never read. It is however used in grafana, and we thus
    // read it at a given frequency to save the data to the database
    void ReadAlstom();

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

private:
    void SetupTopLevelDevices();
    void SetupBottomLevelDevices();
    void SetupStateMachine();
    void PrintStateChanges(QAbstractState *state, QString name);
    void CheckDbus();

    QMap<QString, Device*> toplevel_devices_;
    QMap<QString, Device*> bottomlevel_devices_;
    bool dbus_connected_ = false;

    QStateMachine sm_;

    medicyc::cyclotron::ADCInterface adc_;
    medicyc::cyclotron::OmronInterface omron_;
    medicyc::cyclotron::MessageLoggerInterface logger_;

    // OMRON GLOBAL COMMANDS
    const QString CMD_COMMANDALLON = "C On Alimentations";
    const QString CMD_COMMANDALLOFF = "C Off Alimentations";
    const QString CMD_COMMANDPREAMPLISON = "C On PreAmpli";
    const QString CMD_COMMANDPREAMPLISOFF = "C Off Totale PreAmpli";
    const QString CMD_COMMANDAMPLISON = "C On Ampli";
    const QString CMD_COMMANDAMPLISOFF = "C Off Ampli";

    const QString CMD_ISSTARTUPONGOING = "Q Demarrage Alims en Cours";
    const QString CMD_ISSTARTUPFINISHED = "Q Demarrage Alims Termine";
    const QString CMD_ISALLON = "Q Is All On";
    const QString CMD_ISALLOFF = "Q Is All Off";
    const QString CMD_ISDEFAUTACTIVE = "Q Is Defaut Active";
    const int PULSE_WIDTH = 50;

    bool defaut_active_ = false;
    bool all_on_ = false;
    bool all_off_ = false;
};

}

#endif
