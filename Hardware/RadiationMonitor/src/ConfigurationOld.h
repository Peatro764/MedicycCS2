#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <QMap>

#include "Definitions.h"
#include "Calc.h"
#include "BaliseConfiguration.h"

class Configuration : public QObject {
    Q_OBJECT
public:
    Configuration();
    Configuration(BaliseConfiguration balise_configuration);
    ~Configuration();

    QMap<QString, QString> GetWrongParameters();
    bool IsInitialised() { return is_initialised_; }

    ModeFunctionnement GetCorrectModeFunctionnement() const { return static_cast<ModeFunctionnement>(balise_configuration_.ModeFunctionnement()); }
    PreAmpliType GetCorrectPreAmpliType() const { return static_cast<PreAmpliType>(balise_configuration_.PreAmpliType()); }
    PreAmpliGamme GetCorrectPreAmpliGamme() const { return static_cast<PreAmpliGamme>(balise_configuration_.PreAmpliGamme()); }
    bool GetCorrectInstantaneousLAM() const { return balise_configuration_.InstantaneousLAM(); }
    bool GetCorrectIntegrated1LAM() const { return balise_configuration_.Integrated1LAM(); }
    bool GetCorrectIntegrated2LAM() const { return balise_configuration_.Integrated2LAM(); }
    bool GetCorrectCommandConfirmation() const { return balise_configuration_.CommandConfirmation(); }
    bool GetCorrectIncludeDateInMeasurement() const { return balise_configuration_.IncludeDateInMeasurement(); }
    bool GetCorrectIncludeRawInMeasurement() const { return balise_configuration_.IncludeRawInMeasurement(); }
    int GetCorrectInstantaneousTime() const { return balise_configuration_.InstantaneousTime(); }
    int GetCorrectInstantaneousElements() const { return balise_configuration_.InstantaneousElements(); }
    int GetCorrectIntegrated1Time() const { return balise_configuration_.Integrated1Time(); }
    int GetCorrectIntegrated2Time() const { return balise_configuration_.Integrated2Time(); }
    int GetCorrectNumberOfMeasurements() const { return balise_configuration_.NumberOfMeasurements(); }
    double GetCorrectInstantaneousConversionCoefficient() const { return balise_configuration_.InstantaneousConversionCoefficient(); }
    double GetCorrectIntegratedConversionCoefficient() const { return balise_configuration_.IntegratedConversionCoefficient(); }
    double GetCorrectIntegratedThreshold() const { return balise_configuration_.IntegratedThreshold(); }
    double GetCorrectThresholdA() const { return balise_configuration_.ThresholdA(); }
    double GetCorrectThresholdB() const { return balise_configuration_.ThresholdB(); }
    double GetCorrectThresholdC() const { return balise_configuration_.ThresholdC(); }

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
    void ModeFunctionnementChanged(ModeFunctionnement mode) { mode_functionnement_correct_ = (mode == static_cast<ModeFunctionnement>(balise_configuration_.ModeFunctionnement()));
                                                              mode_functionnement_read_ = true; CheckIfOk(); }
    void PreAmpliTypeChanged(PreAmpliType type) { preampli_type_correct_ = (type == static_cast<PreAmpliType>(balise_configuration_.PreAmpliType()));
                                                  preampli_type_read_ = true;  CheckIfOk(); }
    void PreAmpliGammeChanged(PreAmpliGamme gamme) { preampli_gamme_correct_ = (gamme == static_cast<PreAmpliGamme>(balise_configuration_.PreAmpliGamme()));
                                                     preampli_gamme_read_ = true; CheckIfOk(); }
    void InstantaneousLAMChanged(bool lam) { instantaneous_lam_correct_ = (lam == balise_configuration_.InstantaneousLAM());
                                             instantaneous_lam_read_ = true;  CheckIfOk(); }
    void Integrated1LAMChanged(bool lam) { integrated1_lam_correct_ = (lam == balise_configuration_.Integrated1LAM());
                                           integrated1_lam_read_ = true; CheckIfOk(); }
    void Integrated2LAMChanged(bool lam) { integrated2_lam_correct_ = (lam == balise_configuration_.Integrated2LAM());
                                           integrated2_lam_read_ = true; CheckIfOk(); }
    void CommandConfirmationChanged(bool confirmation) { command_confirmation_correct_ = (confirmation == balise_configuration_.CommandConfirmation());
                                                         command_confirmation_read_ = true; } // no checkifok here, since this command is always sent
    void IncludeDateInMeasurementChanged(bool date) { include_date_in_measurement_correct_ = (date == balise_configuration_.IncludeDateInMeasurement());
                                                      include_date_in_measurement_read_ = true; CheckIfOk(); }
    void IncludeRawInMeasurementChanged(bool raw) { include_raw_in_measurement_correct_ = (raw == balise_configuration_.IncludeRawInMeasurement());
                                                    include_raw_in_measurement_read_ = true; CheckIfOk(); }

    // numeric parameter replies
    void InstantaneousTimeChanged(int seconds) { instantaneous_time_correct_ = (seconds == balise_configuration_.InstantaneousTime());
                                                 instantaneous_time_read_ = true; CheckIfOk(); }
    void InstantaneousElementsChanged(int elements) { instantaneous_elements_correct_ = (elements == balise_configuration_.InstantaneousElements());
                                                      instantaneous_elements_read_ = true; CheckIfOk(); }
    void Integrated1TimeChanged(int minutes) { integrated1_time_correct_ = (minutes == balise_configuration_.Integrated1Time());
                                               integrated1_time_read_ = true; CheckIfOk(); }
    void Integrated2TimeChanged(int hours) { integrated2_time_correct_ = (hours == balise_configuration_.Integrated2Time());
                                             integrated2_time_read_ = true; CheckIfOk(); }
    void NumberOfMeasurementsChanged(int measurements) { number_of_measurements_correct_ = (measurements == balise_configuration_.NumberOfMeasurements());
                                                         number_of_measurements_read_ = true; CheckIfOk(); }
    void InstantaneousConversionCoefficientChanged(double coeff) { instantaneous_conversion_coefficient_correct_ = calc::AlmostEqual(coeff , balise_configuration_.InstantaneousConversionCoefficient(), 0.0001);
                                                                   instantaneous_conversion_coefficient_read_ = true; CheckIfOk(); }
    void IntegratedConversionCoefficientChanged(double coeff) { integrated_conversion_coefficient_correct_ = calc::AlmostEqual(coeff, balise_configuration_.IntegratedConversionCoefficient(), 0.0001);
                                                                integrated_conversion_coefficient_read_ = true; CheckIfOk(); }
    void IntegratedThresholdChanged(double threshold) { integrated_threshold_correct_ = calc::AlmostEqual(threshold, balise_configuration_.IntegratedThreshold(), 0.0001);
                                                        integrated_threshold_read_ = true; CheckIfOk(); }
    void ThresholdAChanged(double threshold) { threshold_A_correct_ = calc::AlmostEqual(threshold, balise_configuration_.ThresholdA(), 0.0001);
                                               threshold_A_read_ = true; CheckIfOk(); }
    void ThresholdBChanged(double threshold) { threshold_B_correct_ = calc::AlmostEqual(threshold, balise_configuration_.ThresholdB(), 0.0001);
                                               threshold_B_read_ = true; CheckIfOk(); }
    void ThresholdCChanged(double threshold) { threshold_C_correct_ = calc::AlmostEqual(threshold, balise_configuration_.ThresholdC(), 0.0001);
                                               threshold_C_read_ = true; CheckIfOk(); }

private slots:

signals:
    void Ok();
    void Wrong();

private:
    void CheckIfOk();
    bool StatusRead() const;

    BaliseConfiguration balise_configuration_;
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

#endif
