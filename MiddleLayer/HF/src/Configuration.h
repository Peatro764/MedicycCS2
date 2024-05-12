#ifndef MIDDLELAYER_HF_CONFIGURATION_H
#define MIDDLELAYER_HF_CONFIGURATION_H

#include <QDateTime>
#include <iostream>

#include "HFUtils.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

class Configuration {

public:
    Configuration();
    Configuration(HFX hf,
                  QDateTime timestamp,
                  double accord_voltage_lower,
                  double accord_voltage_upper,
                  double accord_phase_lower,
                  double accord_phase_upper,
                  double membrane_min,
                  double membrane_max,
                  double membrane_clearance,
                  double niveau_preon_value,
                  double niveau_preoff_value,
                  double niveau_startup_final_value,
                  double niveau_shutdown_initial_value,
                  double niveau_max,
                  double voltage_max,
                  double voltage_delta_down,
                  double voltage_delta_up,
                  double voltage_post_accord);
    ~Configuration();

    HFX hf() const { return hf_; }
    QDateTime timestamp() const { return timestamp_; }
    double accord_voltage_upper() const { return accord_voltage_upper_; }
    double accord_voltage_lower() const { return accord_voltage_lower_; }
    double accord_phase_lower() const { return accord_phase_lower_; }
    double accord_phase_upper() const { return accord_phase_upper_; }
    double membrane_min() const { return membrane_min_; }
    double membrane_max() const { return membrane_max_; }
    double membrane_clearance() const { return membrane_clearance_; }
    double niveau_preon_value() const { return niveau_preon_value_; }
    double niveau_preoff_value() const { return niveau_preoff_value_; }
    double niveau_startup_final_value() const { return niveau_startup_final_value_; }
    double niveau_shutdown_initial_value() const { return niveau_shutdown_initial_value_; }
    double niveau_max() const { return niveau_max_; }
    double voltage_max() const { return voltage_max_; }
    double voltage_delta_up() const { return voltage_delta_up_; }
    double voltage_delta_down() const { return voltage_delta_down_; }
    double voltage_post_accord() const { return voltage_post_accord_; }

    void update_timestamp() { timestamp_ = QDateTime::currentDateTime(); }
    void set_accord_voltage_upper(double value) { accord_voltage_upper_ = value; }
    void set_accord_voltage_lower(double value) { accord_voltage_lower_ = value; }
    void set_accord_phase_upper(double value) { accord_phase_upper_ = value; }
    void set_accord_phase_lower(double value) { accord_phase_lower_ = value; }
    void set_membrane_min(double value) { membrane_min_ = value; }
    void set_membrane_max(double value) { membrane_max_ = value; }
    void set_membrane_clearance(double value) { membrane_clearance_ = value; }
    void set_niveau_preon_value(double value) { niveau_preon_value_ = value; }
    void set_niveau_preoff_value(double value) { niveau_preoff_value_ = value; }
    void set_niveau_startup_final_value(double value) { niveau_startup_final_value_ = value; }
    void set_niveau_shutdown_initial_value(double value) { niveau_shutdown_initial_value_ = value; }
    void set_niveau_max(double value) { niveau_max_ = value; }
    void set_voltage_max(double value) { voltage_max_ = value; }
    void set_voltage_delta_down(double value) { voltage_delta_down_ = value; }
    void set_voltage_delta_up(double value) { voltage_delta_up_ = value; }
    void set_voltage_post_accord(double value) { voltage_post_accord_ = value; }
private:
    HFX hf_;
    QDateTime timestamp_;
    double accord_voltage_lower_ = 0.0;
    double accord_voltage_upper_ = 0.0;
    double accord_phase_lower_= 0.0;
    double accord_phase_upper_= 0.0;
    double membrane_min_ = 0.0;
    double membrane_max_ = 0.0;
    double membrane_clearance_ = 0.0;
    double niveau_preon_value_ = 0.0;
    double niveau_preoff_value_ = 0.0;
    double niveau_startup_final_value_ = 0.0;
    double niveau_shutdown_initial_value_ = 0.0;
    double niveau_max_ = 0.0;
    double voltage_max_ = 0.0;
    double voltage_delta_down_ = 0.0;
    double voltage_delta_up_ = 0.0;
    double voltage_post_accord_ = 0.0;
};

}

#endif
