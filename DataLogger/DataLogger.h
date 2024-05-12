#ifndef DATALOGGER_H
#define DATALOGGER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QString>
#include <QSettings>

#include "NSingleRepo.h"
#include "RadiationMonitorRepo.h"
#include "OmronRepo.h"

#include "nsinglecontroller_interface.h"
#include "electrometer_interface.h"
#include "radiationmonitor_interface.h"
#include "adc_interface.h"
#include "omron_interface.h"
#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::datalogger {

struct DatabaseConfig {
    QString host_name;
    QString database_name;
    int port;
    QString username;
    QString password;
    int connection_timeout;
};

class Worker : public QObject {
  Q_OBJECT

   public:
    Worker(DatabaseConfig config, QObject* parent = 0);
    ~Worker();

public:
    void Connect();
    void SaveMeasurement(QString channel, double value);

private slots:
    void ErrorMessage(QString message);
    void InfoMessage(QString message);

signals:
    void SIGNAL_ErrorMessage(QString message);

private:
    DatabaseConfig config_;
    bool error_active_ = false;
};


class DataLogger : public QThread {
    Q_OBJECT
public:
    DataLogger(QString db_suffix);
     ~DataLogger();

public slots:

private slots:
    void ConnectNSingles();
    void ConnectElectrometers();
    void ConnectRadiationMonitors();
    void ConnectADC();
    void ConnectOmrons();
    void SaveMeasurement(QString channel, double value);

signals: // public
    void SIGNAL_ErrorMessage(QString message);

signals: // private
    void SIGNAL_SaveMeasurement(QString channel, double value);

private:
    void run() override;
    const QString DB_BASE_NAME_ = "timeseriesdatabase";
    QString db_name_;
    DatabaseConfig config_;
    Worker* worker_ = nullptr;
    QSettings settings_;
    medicyc::cyclotron::MessageLoggerInterface logger_;

    QMap<QString, medicyc::cyclotron::NSingleControllerInterface*> nsingle_interfaces_;
    medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo nsingle_repo_;

    QMap<QString, medicyc::cyclotron::RadiationMonitorInterface*> radiation_monitor_interfaces_;
    medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::RadiationMonitorRepo radiation_monitor_repo_;

    QMap<QString, medicyc::cyclotron::OmronInterface*> omron_interfaces_;
    QMap<QString, bool> omron_channel_states_;
    medicyc::cyclotroncontrolsystem::hardware::omron::OmronRepo omron_repo_;

    QMap<QString, medicyc::cyclotron::ElectrometerInterface*> electrometer_interfaces_;
};

} // ns

#endif
