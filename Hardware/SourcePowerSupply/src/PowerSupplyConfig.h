#ifndef POWERSUPPLYCONFIG_H
#define POWERSUPPLYCONFIG_H

#include <QObject>
#include <QString>
#include <QDebug>

#include "RegulatedParameterConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::sourcepowersupply {

class PowerSupplyConfig {
public:
    // name = Extraction, Arc, Filament
    // actuating = I or V (the parameter which will be acted on to change the state of the power supply)
    // secondary = I or V (the parameter which will not be acted on)
    // regulated = V.Extraction Source, I.Arc Source etc (the variable we are trying to regulated, often the same as the actuating variable)
    PowerSupplyConfig();
    PowerSupplyConfig(
            QString name,
            QString regulated_param,
            int heat_up_time,
            RegulatedParameterConfig regulated_param_config,
            RegulatedParameterConfig actuated_param_config,
            QString voltage_unit, QString current_unit);

    QString name() const { return name_; }
    QString regulated_param() const { return regulated_param_; }
    int heat_up_time() const { return heat_up_time_; }
    RegulatedParameterConfig regulated_param_config() const { return regulated_param_config_; }
    RegulatedParameterConfig actuated_param_config() const { return actuated_param_config_; }
    QString voltage_unit() const { return voltage_unit_; }
    QString current_unit() const { return current_unit_; }
    void print() const;

private:
    friend bool operator==(const PowerSupplyConfig &c1, const PowerSupplyConfig& c2);
    QString name_;
    QString regulated_param_;
    int heat_up_time_;
    RegulatedParameterConfig regulated_param_config_;
    RegulatedParameterConfig actuated_param_config_;
    QString voltage_unit_;
    QString current_unit_;
};

bool operator==(const PowerSupplyConfig &c1, const PowerSupplyConfig& c2);
bool operator!=(const PowerSupplyConfig &c1, const PowerSupplyConfig& c2);

}

#endif
