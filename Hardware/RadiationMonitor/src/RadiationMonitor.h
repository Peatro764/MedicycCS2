#ifndef MEDICYCS2_RadiationMonitor_H
#define MEDICYCS2_RadiationMonitor_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QStateMachine>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QTimer>

#include "ReplyParser.h"
#include "CommandCreator.h"
#include "Measurement.h"
#include "Command.h"
#include "MeasurementRingBuffer.h"
#include "XRayIdentifier.h"
#include "ConfigurationComparator.h"
#include "Configuration.h"
#include "SocketClient.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class RadiationMonitor : public QObject
{
    Q_OBJECT
public:
    RadiationMonitor(Configuration configuration);
    ~RadiationMonitor();

public slots:
    // Commands to balise microcontroller. Only used by direct gui, not shared over dbus

    // DBUS commands
    void StartChargeMeasurement() { emit SIGNAL_StartChargeMeasurement(); }
    void StopChargeMeasurement() { emit SIGNAL_StopChargeMeasurement(); }

    // Pilotage Commands
    void PowerOn();
    void StartMeasurement();
    void StopMeasurement();
    void SetSeparator(QString sep);
    void EnableRemote(bool wait_confirmation);
    void EnableLocal();
    void EnableReply();
    void DisableReply();

    // Mode Commands
    void SetModeFunctionnement(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::ModeFunctionnement mode);
    void SetPreAmpliType(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliType type);
    void SetPreAmpliGamme(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliGamme gamme);
    void SetEmitInstanenousMeasurement(bool yes);
    void SetEmitIntegrated1Measurement(bool yes);
    void SetEmitIntegrated2Measurement(bool yes);
    void SetIncludeDateInReply(bool yes);
    void SetIncludeRawMeasurementInReply(bool yes);

    // Parameter Commands
    void SetInstantenousMeasurementTime(int seconds);
    void SetInstantenousMeasurementElements(int elements);
    void SetNmbMeasurementsBelowThresholdBeforeAlarmReset(int measurements);
    void SetIntegratedMeasurement1Time(int minutes);
    void SetIntegratedMeasurement2Time(int hours);
    void SetNmbMeasurementsIntegrationTime2(int measurements);
    void SetInstantenousMeasurementConversionCoefficient(double coeff);
    void SetIntegratedMeasurementConversionCoefficient(double coeff);
    void SetIntegratedMeasurementThresholdLevel(double ampere);
    void SetWarningThreshold1(double ampere);
    void SetWarningThreshold2(double ampere);
    void SetWarningThreshold3(double ampere);

    // Read commands
    void ReadDate();
    void SetDate(QDateTime timestamp);
    void ReadModeParameters(); // LP0
    void ReadNumericParameters(); // LP1
    void ReadThresholdStatus(); // LST
    void ReadInstantenousMeasurement(); // L0I
    void ReadIntegratedMeasurement1(); // L1I
    void ReadIntegratedMeasurement2(); // L2I
    void ReadBufferIntegratedMeasurement1(); // BU1
    void ReadBufferIntegratedMeasurement2(); // BU2

private slots:
    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

    void AddIntegratedToRingBuffer(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurement m);
    void AddBufferedToRingBuffer(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurementBuffer b);

    // Configuration statemachine
    void Configure() { emit SIGNAL_StartConfiguration(); }
    void EnableRemoteCommunication();
    void ReadConfiguration();
    void WriteConfiguration();

    // Messaging statemachines
    void CommandStateMachine(QString message) { qDebug() << QDateTime::currentDateTime() << " CommandStateMachine " << message; }
    void ConfiguringStateMachine(QString message) { qDebug() << "ConfiguringStateMachine " << message; }
    void ChargeMeasurementStateMachine(QString message) { qDebug() << "ChargeMeasurementStateMachine " << message; }
    void ReadBufferStateMachine(QString message) { qDebug() << "ReadBufferStateMachine " << message; }

    // Command execution
    void SendNextCommand();
    void EmptyCommandQueue() { command_list_.clear(); }

    // Dose and xray measurements
    double GetIntegratedCharge(QDateTime time1, QDateTime time2);
    void BroadcastChargeMeasurement();
    void HandleDetectedXRay(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::InstantenousMeasurement peak_value);

signals:
    // Socket connection signals
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_Reconnect();

    // Configuration statemachine signals
    void SIGNAL_StartConfiguration();
    void SIGNAL_ConfiguringDone();
    void SIGNAL_AbortConfiguration();

    // Command statemachine signals
    void SIGNAL_CommandAdded();
    void SIGNAL_CommandSentNoReply();
    void SIGNAL_CommandSentConfirmation();
    void SIGNAL_CommandSentReply();

    // readbuffer statemachine
    void SIGNAL_FailedSecureReadIntegratedBuffer();

    // Charge measurement statemachine signals
    void SIGNAL_StartChargeMeasurement();
    void SIGNAL_StopChargeMeasurement();
    void SIGNAL_AbortChargeMeasurement(); // DBUS
    void SIGNAL_MeasurementInProgress(); // DBUS
    void SIGNAL_MeasurementStopped(); // DBUS

    // DBUS
    void SIGNAL_DoseRate(double value);
    void SIGNAL_IntegratedChargeContinous(double charge);
    void SIGNAL_IntegratedChargeOverInterval(double charge);
    void SIGNAL_XRayDetected(double peak_dose_rate, double integrated_charge);

    // measurement replies
    void InstantenousMeasurementChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::InstantenousMeasurement m);
    void IntegratedMeasurement1Changed(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurement m);
    void IntegratedMeasurement2Changed(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurement m);
    void BufferIntegratedMeasurement1Changed(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurementBuffer b);
    void BufferIntegratedMeasurement2Changed(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::IntegratedMeasurementBuffer b);
    void ThresholdStatusChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::ThresholdStatus status);

    // Mode parameter replies
    void ModeFunctionnementChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::ModeFunctionnement mode);
    void PreAmpliTypeChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliType type);
    void PreAmpliGammeChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliGamme gamme);
    void EmitInstantenousMeasurementChanged(bool yes);
    void EmitIntegrated1MeasurementChanged(bool yes);
    void EmitIntegrated2MeasurementChanged(bool yes);
    void IncludeDateInMeasurementChanged(bool yes);
    void IncludeConversedValueInMeasurementChanged(bool yes);
    void DateChanged(QDateTime date);

    // Numeric parameter replies
    void InstantenousMeasurementTimeChanged(int seconds);
    void InstantenousMeasurementElementsChanged(int elements);
    void NmbMeasurementsBelowThresholdBeforeAlarmResetChanged(int measurements); // n
    void IntegratedMeasurement1TimeChanged(int minutes);
    void IntegratedMeasurement2TimeChanged(int hours);
    void NmbMeasurementsIntegrationTime2Changed(int measurements); // n
    void InstantenousMeasurementConversionCoeffChanged(double coeff);
    void IntegratedMeasurementConversionCoeffChanged(double coeff);
    void IntegratedMeasurementThresholdLevelChanged(double ampere);
    void WarningThreshold1Changed(double conv_value); // n
    void WarningThreshold2Changed(double conv_value); // n
    void WarningThreshold3Changed(double conv_value); // n

private:
    void ConnectClientConnectionSignals();
    void ConnectReplyParserSignals();
    void ConnectSignals();

    void SetupCommandStateMachine();
    void SetupConfiguringStateMachine();
    void SetupChargeMeasurementStateMachine();
    void SetupReadBufferStateMachine();
    void SetSeparator();
    void AddCommand(Command command, bool force = false);
    void ForceAddCommand(Command command);

    QSettings settings_;
    ReplyParser reply_parser_;
    CommandCreator command_creator_;
    ConfigurationComparator configuration_comparator_;
    SocketClient socket_;

    const QString cmd_footer_ = "\r\n";
    QString cmd_separator_ = ":";

    QStateMachine configuring_sm_;
    QStateMachine command_sm_;
    QStateMachine charge_measurement_sm_;
    QStateMachine readBuffer_sm_;
    QList<Command> command_list_;
    bool check_configuration_ = false;

    double inst_conversion_coeff_value_ = 1.0;
    bool inst_conversion_coeff_init_ = false;
    int n_buffer_read_tries_ = 0;
    MeasurementRingBuffer ring_buffer_;
    bool integrated_charge_measurement_enable_ = false;
    XRayIdentifier xray_identifier_;

    struct ChargeMeasurementParams {
        QDateTime start;
        QDateTime stop;
    } charge_measurement_params_;

    int reconnect_interval_ = 3000;
    medicyc::cyclotron::MessageLoggerInterface logger_;
};

}

#endif
