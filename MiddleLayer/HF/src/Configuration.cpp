#include "Configuration.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

Configuration::Configuration() {}

Configuration::Configuration(HFX hf,
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
              double voltage_post_accord)
    : hf_(hf),
      timestamp_(timestamp),
      accord_voltage_lower_(accord_voltage_lower),
      accord_voltage_upper_(accord_voltage_upper),
      accord_phase_lower_(accord_phase_lower),
      accord_phase_upper_(accord_phase_upper),
      membrane_min_(membrane_min),
      membrane_max_(membrane_max),
      membrane_clearance_(membrane_clearance),
      niveau_preon_value_(niveau_preon_value),
      niveau_preoff_value_(niveau_preoff_value),
      niveau_startup_final_value_(niveau_startup_final_value),
      niveau_shutdown_initial_value_(niveau_shutdown_initial_value),
      niveau_max_(niveau_max),
      voltage_max_(voltage_max),
      voltage_delta_down_(voltage_delta_down),
      voltage_delta_up_(voltage_delta_up),
      voltage_post_accord_(voltage_post_accord) {
    qDebug() << "Configuration " << static_cast<int>(hf_) << " Voltage post accord " << voltage_post_accord_ ;
}



Configuration::~Configuration() {

}

}
