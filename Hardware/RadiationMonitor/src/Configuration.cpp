#include "Configuration.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

Configuration::Configuration() {}

Configuration::Configuration(QDateTime timestamp, QString name, QString ip, int port, int connection_timeout, int read_timeout, int write_timeout,
              int mode_functionnement, int preampli_type, int preampli_gamme, bool instantaneous_lam,
              bool integrated1_lam, bool integrated2_lam, bool command_confirmation, bool include_date_in_measurement,
              bool include_raw_in_measurement, int instantaneous_time, int instantaneous_elements,
              int integrated1_time, int integrated2_time, int number_of_measurements,
              double instantaneous_conversion_coefficient, double integrated_conversion_coefficient, double integrated_threshold,
              double threshold_A, double threshold_B, double threshold_C)
    : timestamp_(timestamp), name_(name), ip_(ip), port_(port), connection_timeout_(connection_timeout), read_timeout_(read_timeout),
      write_timeout_(write_timeout), mode_functionnement_(mode_functionnement), preampli_type_(preampli_type), preampli_gamme_(preampli_gamme), instantaneous_lam_(instantaneous_lam),
      integrated1_lam_(integrated1_lam), integrated2_lam_(integrated2_lam), command_confirmation_(command_confirmation),
      include_date_in_measurement_(include_date_in_measurement), include_raw_in_measurement_(include_raw_in_measurement),
      instantaneous_time_(instantaneous_time), instantaneous_elements_(instantaneous_elements), integrated1_time_(integrated1_time),
      integrated2_time_(integrated2_time), number_of_measurements_(number_of_measurements), instantaneous_conversion_coefficient_(instantaneous_conversion_coefficient),
      integrated_conversion_coefficient_(integrated_conversion_coefficient), integrated_threshold_(integrated_threshold),
      threshold_A_(threshold_A), threshold_B_(threshold_B), threshold_C_(threshold_C) {}

Configuration::~Configuration() {}

bool operator==(const Configuration &c1, const Configuration& c2) {
    return (c1.Timestamp() == c2.Timestamp() &&
            c1.Name() == c2.Name() &&
            c1.IP() == c2.IP() &&
            c1.Port() == c2.Port() &&
            c1.ConnectionTimeout() == c2.ConnectionTimeout() &&
            c1.ReadTimeout() == c2.ReadTimeout() &&
            c1.WriteTimeout() == c2.WriteTimeout() &&
            c1.ModeFunctionnement() == c2.ModeFunctionnement() &&
            c1.PreAmpliType() == c2.PreAmpliType() &&
            c1.PreAmpliGamme() == c2.PreAmpliGamme() &&
            c1.InstantaneousLAM() == c2.InstantaneousLAM() &&
            c1.Integrated1LAM() == c2.Integrated1LAM() &&
            c1.Integrated2LAM() == c2.Integrated2LAM() &&
            c1.CommandConfirmation() == c2.CommandConfirmation() &&
            c1.IncludeDateInMeasurement() == c2.IncludeDateInMeasurement() &&
            c1.IncludeRawInMeasurement() == c2.IncludeRawInMeasurement() &&
            c1.InstantaneousTime() == c2.InstantaneousTime() &&
            c1.InstantaneousElements() == c1.InstantaneousElements() &&
            c1.Integrated1Time() == c2.Integrated1Time() &&
            c1.Integrated2Time() == c2.Integrated2Time() &&
            c1.NumberOfMeasurements() == c2.NumberOfMeasurements() &&
            std::abs(c1.InstantaneousConversionCoefficient() - c2.InstantaneousConversionCoefficient()) < 0.001 &&
            std::abs(c1.IntegratedConversionCoefficient() - c2.IntegratedConversionCoefficient()) < 0.001 &&
            std::abs(c1.ThresholdA() - c2.ThresholdA()) < 0.001 &&
            std::abs(c1.ThresholdB() - c2.ThresholdB()) < 0.001 &&
            std::abs(c1.ThresholdC() - c2.ThresholdC()) < 0.001);
}

}
