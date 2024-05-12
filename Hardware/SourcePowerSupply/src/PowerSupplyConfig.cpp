#include "PowerSupplyConfig.h"

#include <numeric>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

PowerSupplyConfig::PowerSupplyConfig() {}

PowerSupplyConfig::PowerSupplyConfig(QString name,
                                     QString regulated_param,
                                     int heat_up_time,
                                     RegulatedParameterConfig regulated_param_config,
                                     RegulatedParameterConfig actuated_param_config,
                                     QString voltage_unit, QString current_unit)
    : name_(name),
      regulated_param_(regulated_param),
      heat_up_time_(heat_up_time),
      regulated_param_config_(regulated_param_config),
      actuated_param_config_(actuated_param_config),
      voltage_unit_(voltage_unit), current_unit_(current_unit) {
}

void PowerSupplyConfig::print() const {
    qDebug() << "name " << name() << "\n"
             << "regulated_param " << regulated_param() << "\n"
             << "heatup time " << heat_up_time() << "\n"
             << "reg_tolerance " << regulated_param_config().tolerance() << "\n"
             << "reg_near_zero " << regulated_param_config().near_zero() << "\n"
             << "reg_near_target " << regulated_param_config().near_target() << "\n"
             << "reg_lower_limit " << regulated_param_config().lower_limit() << "\n"
             << "reg_upper_limit " << regulated_param_config().upper_limit() << "\n"
             << "reg_buffer_size " << regulated_param_config().buffer_size() << "\n"
             << "act_tolerance " << actuated_param_config().tolerance() << "\n"
             << "act_near_zero " << actuated_param_config().near_zero() << "\n"
             << "act_near_target " << actuated_param_config().near_target() << "\n"
             << "act_lower_limit " << actuated_param_config().lower_limit() << "\n"
             << "act_upper_limit " << actuated_param_config().upper_limit() << "\n"
             << "act_buffer_size " << actuated_param_config().buffer_size() << "\n"
             << "voltage unit " << voltage_unit_ << "\n"
             << "current unit " << current_unit_ << "\n";
}


bool operator==(const PowerSupplyConfig &c1, const PowerSupplyConfig& c2) {
    return (c1.name() == c2.name() &&
            c1.regulated_param() == c2.regulated_param() &&
            c1.heat_up_time() == c2.heat_up_time() &&
            c1.regulated_param_config() == c2.regulated_param_config() &&
            c1.actuated_param_config() == c2.actuated_param_config() &&
            c1.voltage_unit() == c2.voltage_unit() &&
            c1.current_unit() == c2.current_unit());
}

bool operator!=(const PowerSupplyConfig &c1, const PowerSupplyConfig& c2) {
    return !(c1 == c2);
}

}
