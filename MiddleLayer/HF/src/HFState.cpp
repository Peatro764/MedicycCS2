#include "HFState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

HFState::HFState(QObject *parent)
    : QObject{parent}
{
    niveau_on_[HFX::HF1] = false;
    niveau_on_[HFX::HF2] = false;
    dee_warm_[HFX::HF1] = false;
    dee_warm_[HFX::HF2] = false;
    regulation_on_[HFX::HF1] = false;
    regulation_on_[HFX::HF2] = false;

    timeout_.setInterval(60000);
    timeout_.setSingleShot(false);
    timeout_.start();
    QObject::connect(&timeout_, &QTimer::timeout, this, [&]() { emit SIGNAL_Unknown(); });

    emit SIGNAL_Unknown();
}

void HFState::SetNiveauOn(HFX hf, bool on) {
    niveau_on_[hf] = on;
    CheckState();
}

void HFState::SetDeeVoltage(HFX hf, double voltage) {
    dee_warm_[hf] = (voltage >= DEE_VOLTAGE_WARM);
    CheckState();
}

void HFState::SetRegulation(HFX hf, bool on) {
    regulation_on_[hf] = on;
    CheckState();
}

void HFState::SetFrequencyWarm(bool state) {
    frequency_warm_ = state;
    CheckState();
}

void HFState::SetFrequencyCold(bool state) {
    frequency_cold_ = state;
    CheckState();
}

void HFState::SetPowerSuppliesAllOn(bool state) {
    powersupply_all_on_ = state;
    CheckState();
}

void HFState::SetPowerSuppliesAllOff(bool state) {
    powersupply_all_off_ = state;
    CheckState();
}

void HFState::CheckState() {
    if (niveau_on_[HFX::HF1] &&
        niveau_on_[HFX::HF2] &&
        dee_warm_[HFX::HF1] &&
        dee_warm_[HFX::HF2] &&
        regulation_on_[HFX::HF1] &&
        regulation_on_[HFX::HF2] &&
        frequency_warm_ &&
        powersupply_all_on_) {
        emit SIGNAL_On();
    } else if (!niveau_on_[HFX::HF1] &&
               !niveau_on_[HFX::HF2] &&
               !dee_warm_[HFX::HF1] &&
               !dee_warm_[HFX::HF2] &&
               !regulation_on_[HFX::HF1] &&
               !regulation_on_[HFX::HF2] &&
               frequency_cold_ &&
               powersupply_all_off_) {
        emit SIGNAL_Off();
    } else {
        emit SIGNAL_Intermediate();
    }
    timeout_.start();
}

} // ns
