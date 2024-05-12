#ifndef MEDICYCCS2_RADIATIONMONITOR_CONFIGURATIONCOMPARATOR_H
#define MEDICYCCS2_RADIATIONMONITOR_CONFIGURATIONCOMPARATOR_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QMap>

#include "Definitions.h"
#include "Calc.h"
#include "Configuration.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

namespace global = medicyc::cyclotroncontrolsystem::global;

class ConfigurationComparator : public QObject {
    Q_OBJECT
public:
    ConfigurationComparator();
    ConfigurationComparator(Configuration desired_configuration);
    ~ConfigurationComparator();

    Configuration DesiredConfiguration() { return desired_configuration_; }
    QMap<QString, QString> GetWrongParameters();
    bool IsInitialised() { return is_initialised_; }

    ModeFunctionnement GetCorrectModeFunctionnement() const { return static_cast<ModeFunctionnement>(desired_configuration_.ModeFunctionnement()); }
    PreAmpliType GetCorrectPreAmpliType() const { return static_cast<PreAmpliType>(desired_configuration_.PreAmpliType()); }
    PreAmpliGamme GetCorrectPreAmpliGamme() const { return static_cast<PreAmpliGamme>(desired_configuration_.PreAmpliGamme()); }
    bool GetCorrectInstantaneousLAM() const { return desired_configuration_.InstantaneousLAM(); }
    bool GetCorrectIntegrated1LAM() const { return desired_configuration_.Integrated1LAM(); }
    bool GetCorrectIntegrated2LAM() const { return desired_configuration_.Integrated2LAM(); }
    bool GetCorrectCommandConfirmation() const { return desired_configuration_.CommandConfirmation(); }
    bool GetCorrectIncludeDateInMeasurement() const { return desired_configuration_.IncludeDateInMeasurement(); }
    bool GetCorrectIncludeRawInMeasurement() const { return desired_configuration_.IncludeRawInMeasurement(); }
    int GetCorrectInstantaneousTime() const { return desired_configuration_.InstantaneousTime(); }
    int GetCorrectInstantaneousElements() const { return desired_configuration_.InstantaneousElements(); }
    int GetCorrectIntegrated1Time() const { return desired_configuration_.Integrated1Time(); }
    int GetCorrectIntegrated2Time() const { return desired_configuration_.Integrated2Time(); }
    int GetCorrectNumberOfMeasurements() const { return desired_configuration_.NumberOfMeasurements(); }
    double GetCorrectInstantaneousConversionCoefficient() const { return desired_configuration_.InstantaneousConversionCoefficient(); }
    double GetCorrectIntegratedConversionCoefficient() const { return desired_configuration_.IntegratedConversionCoefficient(); }
    double GetCorrectIntegratedThreshold() const { return desired_configuration_.IntegratedThreshold(); }
    double GetCorrectThresholdA() const { return desired_configuration_.ThresholdA(); }
    double GetCorrectThresholdB() const { return desired_configuration_.ThresholdB(); }
    double GetCorrectThresholdC() const { return desired_configuration_.ThresholdC(); }

    bool ModeFunctionnementCorrect() { return mode_functionnement_correct_; }
    bool PreAmpliTypeCorrect() { return preampli_type_correct_; }
    bool PreAmpliGammeCorrect() { return preampli_gamme_correct_; }
    bool InstantaneousLAMCorrect() { return instantaneous_lam_correct_; }
    bool Integrated1LAMCorrect() { return integrated1_lam_correct_; }
    bool Integrated2LAMCorrect() { return integrated2_lam_correct_; }
    bool CommandConfirmationCorrect() { return command_confirmation_correct_; }
    bool IncludeDateInMeasurementCorrect() { return include_date_in_measurement_correct_; }
    bool IncludeRawInMeasurementCorrect() { return include_raw_in_measurement_correct_; }
    bool InstantaneousTimeCorrect() { return instantaneous_time_correct_; }
    bool InstantaneousElementsCorrect() { return instantaneous_elements_correct_; }
    bool Integrated1TimeCorrect() { return integrated1_time_correct_; }
    bool Integrated2TimeCorrect() { return integrated2_time_correct_; }
    bool NumberOfMeasurementsCorrect() { return number_of_measurements_correct_; }
    bool InstantaneousConversionCoefficientCorrect() { return instantaneous_conversion_coefficient_correct_; }
    bool IntegratedConversionCoefficientCorrect() { return integrated_conversion_coefficient_correct_; }
    bool IntegratedThresholdCorrect() { return integrated_threshold_correct_; }
    bool ThresholdACorrect() { return threshold_A_correct_; }
    bool ThresholdBCorrect() { return threshold_B_correct_; }
    bool ThresholdCCorrect() { return threshold_C_correct_; }

public slots:
    void ResetStatus() { mode_functionnement_correct_ = false;
                         preampli_type_correct_ = false;
                         preampli_gamme_correct_ = false;
                         instantaneous_lam_correct_ = false;
                         integrated1_lam_correct_ = false;
                         integrated2_lam_correct_ = false;
                         command_confirmation_correct_ = false;
                         include_date_in_measurement_correct_ = false;
                         include_raw_in_measurement_correct_ = false;
                         instantaneous_time_correct_ = false;
                         instantaneous_elements_correct_ = false;
                         integrated1_time_correct_ = false;
                         integrated2_time_correct_ = false;
                         number_of_measurements_correct_ = false;
                         instantaneous_conversion_coefficient_correct_ = false;
                         integrated_conversion_coefficient_correct_ = false;
                         integrated_threshold_correct_ = false;
                         threshold_A_correct_ = false;
                         threshold_B_correct_ = false;
                         threshold_C_correct_ = false;
                                          mode_functionnement_read_ = false;
                                          preampli_type_read_ = false;
                                          preampli_gamme_read_ = false;
                                          instantaneous_lam_read_ = false;
                                          integrated1_lam_read_ = false;
                                          integrated2_lam_read_ = false;
                                          command_confirmation_read_ = false;
                                          include_date_in_measurement_read_ = false;
                                          include_raw_in_measurement_read_ = false;
                                          instantaneous_time_read_ = false;
                                          instantaneous_elements_read_ = false;
                                          integrated1_time_read_ = false;
                                          integrated2_time_read_ = false;
                                          number_of_measurements_read_ = false;
                                          instantaneous_conversion_coefficient_read_ = false;
                                          integrated_conversion_coefficient_read_ = false;
                                          integrated_threshold_read_ = false;
                                          threshold_A_read_ = false;
                                          threshold_B_read_ = false;
                                          threshold_C_read_ = false;
                       qDebug() << "Configuration::ResetStatus"; }

    // Mode parameter replies
    void ModeFunctionnementChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::ModeFunctionnement mode) { mode_functionnement_correct_ = (mode == static_cast<ModeFunctionnement>(desired_configuration_.ModeFunctionnement()));
                                                              mode_functionnement_read_ = true; CheckIfOk(); }
    void PreAmpliTypeChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliType type) { preampli_type_correct_ = (type == static_cast<PreAmpliType>(desired_configuration_.PreAmpliType()));
                                                  preampli_type_read_ = true;  CheckIfOk(); }
    void PreAmpliGammeChanged(medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring::PreAmpliGamme gamme) { preampli_gamme_correct_ = (gamme == static_cast<PreAmpliGamme>(desired_configuration_.PreAmpliGamme()));
                                                     preampli_gamme_read_ = true; CheckIfOk(); }
    void InstantaneousLAMChanged(bool lam) { instantaneous_lam_correct_ = (lam == desired_configuration_.InstantaneousLAM());
                                             instantaneous_lam_read_ = true;  CheckIfOk(); }
    void Integrated1LAMChanged(bool lam) { integrated1_lam_correct_ = (lam == desired_configuration_.Integrated1LAM());
                                           integrated1_lam_read_ = true; CheckIfOk(); }
    void Integrated2LAMChanged(bool lam) { integrated2_lam_correct_ = (lam == desired_configuration_.Integrated2LAM());
                                           integrated2_lam_read_ = true; CheckIfOk(); }
    void CommandConfirmationChanged(bool confirmation) { command_confirmation_correct_ = (confirmation == desired_configuration_.CommandConfirmation());
                                                         command_confirmation_read_ = true; } // no checkifok here, since this command is always sent
    void IncludeDateInMeasurementChanged(bool date) { include_date_in_measurement_correct_ = (date == desired_configuration_.IncludeDateInMeasurement());
                                                      include_date_in_measurement_read_ = true; CheckIfOk(); }
    void IncludeRawInMeasurementChanged(bool raw) { include_raw_in_measurement_correct_ = (raw == desired_configuration_.IncludeRawInMeasurement());
                                                    include_raw_in_measurement_read_ = true; CheckIfOk(); }

    // numeric parameter replies
    void InstantaneousTimeChanged(int seconds) { instantaneous_time_correct_ = (seconds == desired_configuration_.InstantaneousTime());
                                                 instantaneous_time_read_ = true; CheckIfOk(); }
    void InstantaneousElementsChanged(int elements) { instantaneous_elements_correct_ = (elements == desired_configuration_.InstantaneousElements());
                                                      instantaneous_elements_read_ = true; CheckIfOk(); }
    void Integrated1TimeChanged(int minutes) { integrated1_time_correct_ = (minutes == desired_configuration_.Integrated1Time());
                                               integrated1_time_read_ = true; CheckIfOk(); }
    void Integrated2TimeChanged(int hours) { integrated2_time_correct_ = (hours == desired_configuration_.Integrated2Time());
                                             integrated2_time_read_ = true; CheckIfOk(); }
    void NumberOfMeasurementsChanged(int measurements) { number_of_measurements_correct_ = (measurements == desired_configuration_.NumberOfMeasurements());
                                                         number_of_measurements_read_ = true; CheckIfOk(); }
    void InstantaneousConversionCoefficientChanged(double coeff) { instantaneous_conversion_coefficient_correct_ = global::AlmostEqual(coeff , desired_configuration_.InstantaneousConversionCoefficient(), 0.0001);
                                                                   instantaneous_conversion_coefficient_read_ = true; CheckIfOk(); }
    void IntegratedConversionCoefficientChanged(double coeff) { integrated_conversion_coefficient_correct_ = global::AlmostEqual(coeff, desired_configuration_.IntegratedConversionCoefficient(), 0.0001);
                                                                integrated_conversion_coefficient_read_ = true; CheckIfOk(); }
    void IntegratedThresholdChanged(double threshold) { integrated_threshold_correct_ = global::AlmostEqual(threshold, desired_configuration_.IntegratedThreshold(), 0.0001);
                                                        integrated_threshold_read_ = true; CheckIfOk(); }
    void ThresholdAChanged(double threshold) { threshold_A_correct_ = global::AlmostEqual(threshold, desired_configuration_.ThresholdA(), 0.0001);
                                               threshold_A_read_ = true; CheckIfOk(); }
    void ThresholdBChanged(double threshold) { threshold_B_correct_ = global::AlmostEqual(threshold, desired_configuration_.ThresholdB(), 0.0001);
                                               threshold_B_read_ = true; CheckIfOk(); }
    void ThresholdCChanged(double threshold) { threshold_C_correct_ = global::AlmostEqual(threshold, desired_configuration_.ThresholdC(), 0.0001);
                                               threshold_C_read_ = true; CheckIfOk(); }

private slots:

signals:
    void Ok();
    void Wrong();

private:
    void CheckIfOk();
    bool StatusRead() const;

    Configuration desired_configuration_;
    bool is_initialised_ = false;

    bool mode_functionnement_correct_ = false;
    bool preampli_type_correct_ = false;
    bool preampli_gamme_correct_ = false;
    bool instantaneous_lam_correct_ = false;
    bool integrated1_lam_correct_ = false;
    bool integrated2_lam_correct_ = false;
    bool command_confirmation_correct_ = false;
    bool include_date_in_measurement_correct_ = false;
    bool include_raw_in_measurement_correct_ = false;
    bool instantaneous_time_correct_ = false;
    bool instantaneous_elements_correct_ = false;
    bool integrated1_time_correct_ = false;
    bool integrated2_time_correct_ = false;
    bool number_of_measurements_correct_ = false;
    bool instantaneous_conversion_coefficient_correct_ = false;
    bool integrated_conversion_coefficient_correct_ = false;
    bool integrated_threshold_correct_ = false;
    bool threshold_A_correct_ = false;
    bool threshold_B_correct_ = false;
    bool threshold_C_correct_ = false;

    bool mode_functionnement_read_ = false;
    bool preampli_type_read_ = false;
    bool preampli_gamme_read_ = false;
    bool instantaneous_lam_read_ = false;
    bool integrated1_lam_read_ = false;
    bool integrated2_lam_read_ = false;
    bool command_confirmation_read_ = false;
    bool include_date_in_measurement_read_ = false;
    bool include_raw_in_measurement_read_ = false;
    bool instantaneous_time_read_ = false;
    bool instantaneous_elements_read_ = false;
    bool integrated1_time_read_ = false;
    bool integrated2_time_read_ = false;
    bool number_of_measurements_read_ = false;
    bool instantaneous_conversion_coefficient_read_ = false;
    bool integrated_conversion_coefficient_read_ = false;
    bool integrated_threshold_read_ = false;
    bool threshold_A_read_ = false;
    bool threshold_B_read_ = false;
    bool threshold_C_read_ = false;
};

}

#endif

