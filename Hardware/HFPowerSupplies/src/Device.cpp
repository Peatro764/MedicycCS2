#include "Device.h"

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

Device::Device(QString name, QString cmd_is_on, QString cmd_is_off, QString cmd_defaut_active, QString cmd_voltage)
    : name_(name), cmd_is_on_(cmd_is_on), cmd_is_off_(cmd_is_off), cmd_defaut_active_(cmd_defaut_active),
      cmd_voltage_(cmd_voltage)
{
}

void Device::SetVoltage(double value) {
    voltage_ = value;
    emit SIGNAL_Voltage(voltage_);
}

void Device::SetOn(bool state) {
    on_ = state;
    EvaluateState();
}

void Device::SetOff(bool state) {
    off_ = state;
    EvaluateState();
}

void Device::SetDefaut(bool state) {
    defaut_ = state;
    defaut_ ? emit SIGNAL_Defaut_Active() : emit SIGNAL_No_Defaut();
}

void Device::EvaluateState() {
    if (on_ && !off_) {
        emit SIGNAL_On();
    } else {
        emit SIGNAL_Off();
    }
}

}

