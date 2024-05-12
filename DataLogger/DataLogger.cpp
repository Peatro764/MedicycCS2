#include "DataLogger.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QMessageBox>
#include <QStandardPaths>

#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::datalogger {

Worker::Worker(DatabaseConfig config, QObject* parent)
    : QObject(parent), config_(config) {
    QSqlDatabase repo = QSqlDatabase::addDatabase("QPSQL7", "WorkerThread");
    repo.setHostName(config_.host_name);
    repo.setPort(config_.port);
    repo.setDatabaseName(config_.database_name);
    repo.setUserName(config_.username);
    repo.setPassword(config_.password);
    repo.setConnectOptions(QString("connect_timeout=") + QString::number(config_.connection_timeout));
}

Worker::~Worker() {
    QSqlDatabase::removeDatabase("WorkerThread");
}

void Worker::Connect() {
    QSqlDatabase SensorRepo = QSqlDatabase::database("WorkerThread");
    if (SensorRepo.open()) {
        InfoMessage("Worker::Worker DataLogger repo connected");
    } else {
        ErrorMessage("Impossible de se connecter à la base de données");
    }
}

void Worker::ErrorMessage(QString message) {
    if (!error_active_) {
        qDebug() << "Worker::ErrorMessage " << message;
        emit SIGNAL_ErrorMessage(message);
        error_active_ = true;
    }
}

void Worker::InfoMessage(QString message) {
    qDebug() << "Worker::InfoMessage " << message;
}

void Worker::SaveMeasurement(QString channel, double value) {
    QSqlDatabase SensorRepo = QSqlDatabase::database("WorkerThread");
    if (!SensorRepo.open()) {
        ErrorMessage("DataLogger db is not open");
        return;
    }

    QSqlQuery query(SensorRepo);
    query.prepare("INSERT INTO Measurement(time, channel, value) "
                  "VALUES(:time, :channel, :value)");
    query.bindValue(":time", QDateTime::currentDateTimeUtc());
    query.bindValue(":channel", channel);
    query.bindValue(":value", value);

    if (query.exec()) {
        error_active_ = false;
    } else {
        ErrorMessage("Failed inserting data in datalogger database");
    }
}

DataLogger::DataLogger(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix),
      settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
               QSettings::IniFormat),
      logger_("medicyc.cyclotron.messagelogger", "/MessageLogger", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
      nsingle_repo_("_prod"),
      radiation_monitor_repo_("_prod"),
      omron_repo_("_prod") {

    QObject::connect(this, &DataLogger::SIGNAL_ErrorMessage, this, [&](QString msg) { logger_.Error(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch(), "DataLogger", msg); });

    config_.host_name = settings_.value(db_name_ + "/host", "unknown").toString();
    config_.database_name = settings_.value(db_name_ + "/name", "unknown").toString();
    config_.port = settings_.value(db_name_ + "/port", "0").toInt();
    config_.username = settings_.value(db_name_ + "/user", "unknown").toString();
    config_.password = settings_.value(db_name_ + "/password", "unknown").toString();

    this->start();

    qDebug() << "DataLogger::DataLogger Host " << config_.host_name
             << " database " << config_.database_name
             << " port " << config_.port
             << " username " << config_.username
             << " password " << config_.password;

    ConnectNSingles();
    ConnectElectrometers();
    ConnectRadiationMonitors();
    ConnectADC();
    ConnectOmrons();
}

DataLogger::~DataLogger() {
    this->quit(); // stops thread
    while (this->isRunning()) {}
    if (worker_) delete worker_;

    for (auto key : nsingle_interfaces_.keys()) {
        delete nsingle_interfaces_.value(key);
    }
    for (auto key : electrometer_interfaces_.keys()) {
        delete electrometer_interfaces_.value(key);
    }
    for (auto key : radiation_monitor_interfaces_.keys()) {
        delete radiation_monitor_interfaces_.value(key);
    }   
}

void DataLogger::ConnectNSingles() {
    try {
        std::vector<QString> nsingles = nsingle_repo_.GetNSingles("ALL");
        for (auto& n : nsingles) {
            auto dbus_address = nsingle_repo_.GetNSingleDbusAddress(n);
            auto controller = new medicyc::cyclotron::NSingleControllerInterface(dbus_address.interface_address(), dbus_address.controller_object_name(),
                                                                                 medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            if (controller->isValid()) {
                qDebug() << "Datalogger::ConnectNSingles Connected to " << n;
            } else {
                qWarning() << "Datalogger::ConnectNSingles Failed connecting to " << n;
            }
            nsingle_interfaces_[n] = controller;
            QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [this, n](double physical_value, bool polarity) {
                this->SaveMeasurement(n + "_Current", polarity ? physical_value : -1*physical_value); });

        }
    } catch (std::exception& exc) {
        nsingle_interfaces_.clear();
        qWarning() << "DataLogger::ConnectNSingles Failed fetching nsingle addresses from db: " << exc.what();
    }
}

void DataLogger::ConnectElectrometers() {
    // TODO: Get all electrometers and loop over names. For the moment the name given below is hardcoded and should match the name in the process db table
    QString name = "Stripper";
    auto stripper_interface = new medicyc::cyclotron::ElectrometerInterface("medicyc.cyclotron.hardware.electrometer." + name, "/" + name,
                                                                            medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
    if (stripper_interface->isValid()) {
        qDebug() << "DataLogger::ConnectElectrometer Connected stripper electrometer";
    } else {
        qWarning() << "DataLogger::ConnectElectrometer Failed connecting to stripper electrometer";
    }
    QObject::connect(stripper_interface, &medicyc::cyclotron::ElectrometerInterface::SIGNAL_Measurement, this, [this](double timestamp, double value, double ol) {
        (void)timestamp;
        (void)ol;
        if (abs(value) < 1.0) {
            this->SaveMeasurement("STRIPPER_CURRENT", value);
        } else {
            qDebug() << "Errenouse measurement " << value;
        }
    });
    electrometer_interfaces_[name] = stripper_interface;
}

void DataLogger::ConnectOmrons() {
    try {
        auto omron_systems = omron_repo_.GetOmronSystemNames();
        for (auto& n : omron_systems) {
            auto interface = new medicyc::cyclotron::OmronInterface(QString("medicyc.cyclotron.hardware.omron.") + n.toLower(), "/Omron",
                                                                    medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            if (interface->isValid()) {
                qDebug() << "Datalogger::ConnectOmrons Connected to " << n;
            } else {
                qWarning() << "Datalogger::ConnectOmrons Failed connecting to " << n;
            }
            omron_interfaces_[n] = interface;
            QObject::connect(interface, &medicyc::cyclotron::OmronInterface::SIGNAL_BitRead, this, [this, n](const QString& channel, bool content) {
                if (!omron_channel_states_.contains(channel)) {
                    this->SaveMeasurement("OMRON_" + n + "_" + channel, content);
                } else if (omron_channel_states_[channel] != content) {
                    this->SaveMeasurement("OMRON_" + n + "_" + channel, content);
                }
                omron_channel_states_[channel] = content;
            });
        }
    } catch (std::exception& exc) {
        omron_interfaces_.clear();
        qWarning() << "DataLogger::ConnectOmrons Excecption thrown: " << exc.what();
    }

}

void DataLogger::ConnectADC() {
    auto adc_interface = new medicyc::cyclotron::ADCInterface("medicyc.cyclotron.hardware.adc", "/ADC",
                                                                            medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
    if (adc_interface->isValid()) {
        qDebug() << "DataLogger::ConnectADC Connected ADC";
    } else {
        qWarning() << "DataLogger::ConnectADC Failed connecting to ADC";
    }
    QObject::connect(adc_interface, &medicyc::cyclotron::ADCInterface::SIGNAL_ReceivedChannelValue,
                     this, [this](const QString &channel, double value, const QString& unit) {
        (void)unit; // no longer saved to database to reduce size
        this->SaveMeasurement(channel, value);
    });
}

void DataLogger::ConnectRadiationMonitors() {
    try {
        std::vector<QString> radiation_monitors = radiation_monitor_repo_.GetRadiationMonitorNames();
        for (auto& n : radiation_monitors) {
            auto interface = new medicyc::cyclotron::RadiationMonitorInterface("medicyc.cyclotron.hardware.radiation_monitor." + n, "/" + n,
                                                                               medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            if (interface->isValid()) {
                qDebug() << "Datalogger::ConnectRadiationMonitors Connected to " << n;
            } else {
                qWarning() << "Datalogger::ConnectRadiationMonitors Failed connecting to " << n;
            }
            radiation_monitor_interfaces_[n] = interface;
            QObject::connect(interface, &medicyc::cyclotron::RadiationMonitorInterface::SIGNAL_DoseRate, this, [this, n](double value) {
                this->SaveMeasurement(n + "_DoseRate", value); });
        }
    } catch (std::exception& exc) {
        radiation_monitor_interfaces_.clear();
        qWarning() << "DataLogger::ConnectRadiationMonitors Excecption thrown: " << exc.what();
    }
}

void DataLogger::SaveMeasurement(QString channel, double value) {
    emit SIGNAL_SaveMeasurement(channel, value);
}

void DataLogger::run() {
  worker_ = new Worker(config_);

  QObject::connect(worker_, &Worker::SIGNAL_ErrorMessage, this, &DataLogger::SIGNAL_ErrorMessage);
  worker_->Connect();

  // forward to the Worker a 'queued connection'!
  QObject::connect(this, &DataLogger::SIGNAL_SaveMeasurement, worker_, &Worker::SaveMeasurement);

  exec();  // start our own event loop
}


} // namespace

