#include "Configuration.h"

Configuration::Configuration(BaliseConfiguration balise_configuration)
    : balise_configuration_(balise_configuration), is_initialised_(true) {}

Configuration::~Configuration() {}

void Configuration::CheckIfOk() {
    if (!StatusRead()) {
        return;
    }
    if (mode_functionnement_correct_ &&
            preampli_type_correct_ &&
            preampli_gamme_correct_ &&
            instantaneous_lam_correct_ &&
            integrated1_lam_correct_ &&
            integrated2_lam_correct_ &&
            command_confirmation_correct_ &&
            include_date_in_measurement_correct_ &&
            include_raw_in_measurement_correct_ &&
            instantaneous_time_correct_ &&
            instantaneous_elements_correct_ &&
            integrated1_time_correct_ &&
            integrated2_time_correct_ &&
            number_of_measurements_correct_ &&
            instantaneous_conversion_coefficient_correct_ &&
            integrated_conversion_coefficient_correct_ &&
            integrated_threshold_correct_ &&
            threshold_A_correct_ &&
            threshold_B_correct_ &&
            threshold_C_correct_) {
        qDebug() << "Configuration::OK";
            emit Ok();
    } else {
        qDebug() << "Configuration::WRONG";
        emit Wrong();
    }
}

bool Configuration::StatusRead() const {
    return (mode_functionnement_read_ &&
            preampli_type_read_ &&
            preampli_gamme_read_ &&
            instantaneous_lam_read_ &&
            integrated1_lam_read_ &&
            integrated2_lam_read_ &&
            command_confirmation_read_ &&
            include_date_in_measurement_read_ &&
            include_raw_in_measurement_read_ &&
            instantaneous_time_read_ &&
            instantaneous_elements_read_ &&
            integrated1_time_read_ &&
            integrated2_time_read_ &&
            number_of_measurements_read_ &&
            instantaneous_conversion_coefficient_read_ &&
            integrated_conversion_coefficient_read_ &&
            integrated_threshold_read_ &&
            threshold_A_read_ &&
            threshold_B_read_ &&
            threshold_C_read_);
}

QMap<QString, QString> Configuration::GetWrongParameters() {
    QMap<QString, QString> wrong_params;
    if (!mode_functionnement_correct_ && mode_functionnement_read_) wrong_params.insert("Mode functionnement", "");
    if (!preampli_type_correct_ && preampli_type_read_) wrong_params.insert("PreAmpliType", "");
    if (!preampli_gamme_correct_ && preampli_gamme_read_) wrong_params.insert("PreAmpliGamme", "");
    if (!instantaneous_lam_correct_ && instantaneous_lam_read_) wrong_params.insert("Instantaneous LAM", "");
    if (!integrated1_lam_correct_ && integrated1_lam_read_) wrong_params.insert("Integrated1 LAM", "");
    if (!integrated2_lam_correct_ && integrated2_lam_read_) wrong_params.insert("Integrated2 LAM", "");
    if (!command_confirmation_correct_ && command_confirmation_read_) wrong_params.insert("Command Confirmation", "");
    if (!include_date_in_measurement_correct_ && include_date_in_measurement_read_) wrong_params.insert("Include date in measurement", "");
    if (!include_raw_in_measurement_correct_ && include_raw_in_measurement_read_) wrong_params.insert("Include raw in measurement", "");
    if (!instantaneous_time_correct_ && instantaneous_time_read_) wrong_params.insert("Instantaneous Time", "");
    if (!instantaneous_elements_correct_ && instantaneous_elements_read_) wrong_params.insert("Instantaneous Elements", "");
    if (!integrated1_time_correct_ && integrated1_lam_read_) wrong_params.insert("Integrated1 Time", "");
    if (!integrated2_time_correct_ && integrated2_time_read_) wrong_params.insert("Integrated2 Time", "");
    if (!number_of_measurements_correct_ && number_of_measurements_read_) wrong_params.insert("Number of measurements", "");
    if (!instantaneous_conversion_coefficient_correct_ && instantaneous_conversion_coefficient_read_) wrong_params.insert("Instantaneous conversion coefficient", "");
    if (!integrated_conversion_coefficient_correct_ && integrated_conversion_coefficient_read_) wrong_params.insert("Integrated conversion coefficient", "");
    if (!integrated_threshold_correct_ && integrated_threshold_read_) wrong_params.insert("Integrated threshold", "");
    if (!threshold_A_correct_ && threshold_A_read_) wrong_params.insert("Threshold A", "");
    if (!threshold_B_correct_ && threshold_B_read_) wrong_params.insert("Threshold B", "");
    if (!threshold_C_correct_ && threshold_C_read_) wrong_params.insert("Threshold C", "");
    return wrong_params;
}
