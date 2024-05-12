#ifndef MEDICYCCS2_RADIATIONMONITOR_CONFIGURATION_H
#define MEDICYCCS2_RADIATIONMONITOR_CONFIGURATION_H

#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class Configuration {

public:
    Configuration();
    Configuration(QDateTime timestamp, QString name, QString ip, int port, int connection_timeout, int read_timeout, int write_timeout,
                  int mode_functionnement, int preampli_type, int preampli_gamme, bool instantaneous_lam,
                  bool integrated1_lam, bool integrated2_lam, bool command_confirmation, bool include_date_in_measurement,
                  bool include_raw_in_measurement, int instantaneous_time, int instantaneous_elements,
                  int integrated1_time, int integrated2_time, int number_of_measurements,
                  double instantaneous_conversion_coefficient, double integrated_conversion_coefficient, double integrated_threshold,
                  double threshold_A, double threshold_B, double threshold_C);
    ~Configuration();

    QDateTime Timestamp() const { return timestamp_; }
    QString Name() const { return name_; }
    QString IP() const { return ip_; }
    int Port() const { return port_; }
    int ConnectionTimeout() const { return connection_timeout_; }
    int ReadTimeout() const { return read_timeout_; }
    int WriteTimeout() const { return write_timeout_; }
    int ModeFunctionnement() const { return mode_functionnement_; }
    int PreAmpliType() const { return preampli_type_; }
    int PreAmpliGamme() const { return preampli_gamme_; }
    bool InstantaneousLAM() const { return instantaneous_lam_; }
    bool Integrated1LAM() const { return integrated1_lam_; }
    bool Integrated2LAM() const { return integrated2_lam_; }
    bool CommandConfirmation() const { return command_confirmation_; }
    bool IncludeDateInMeasurement() const { return include_date_in_measurement_; }
    bool IncludeRawInMeasurement() const { return include_raw_in_measurement_; }
    int InstantaneousTime() const { return instantaneous_time_; }
    int InstantaneousElements() const { return instantaneous_elements_; }
    int Integrated1Time() const { return integrated1_time_; }
    int Integrated2Time() const { return integrated2_time_; }
    int NumberOfMeasurements() const { return number_of_measurements_; }
    double InstantaneousConversionCoefficient() const { return instantaneous_conversion_coefficient_; }
    double IntegratedConversionCoefficient() const { return integrated_conversion_coefficient_; }
    double IntegratedThreshold() const { return integrated_threshold_; }
    double ThresholdA() const { return threshold_A_; }
    double ThresholdB() const { return threshold_B_; }
    double ThresholdC() const { return threshold_C_; }


private:
    QDateTime timestamp_;
    QString name_;
    QString ip_;
    int port_;
    int connection_timeout_;
    int read_timeout_;
    int write_timeout_;
    int mode_functionnement_ = -1;
    int preampli_type_ = -1;
    int preampli_gamme_ = -1;
    bool instantaneous_lam_ = false;
    bool integrated1_lam_ = false;
    bool integrated2_lam_ = false;
    bool command_confirmation_ = false;
    bool include_date_in_measurement_ = false;
    bool include_raw_in_measurement_ = false;
    int instantaneous_time_ = -1;
    int instantaneous_elements_ = -1;
    int integrated1_time_ = -1;
    int integrated2_time_ = 1;
    int number_of_measurements_ = -1;
    double instantaneous_conversion_coefficient_ = -1.0;
    double integrated_conversion_coefficient_ = -1.0;
    double integrated_threshold_ = -1.0;
    double threshold_A_ = -1.0;
    double threshold_B_ = -1.0;
    double threshold_C_ = -1.0;
};

bool operator==(const Configuration &c1, const Configuration& c2);

} // ns

#endif
