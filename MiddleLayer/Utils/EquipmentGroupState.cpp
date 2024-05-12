#include "EquipmentGroupState.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::utils {

EquipmentGroupState::EquipmentGroupState() {}

EquipmentGroupState::~EquipmentGroupState() {}

void EquipmentGroupState::AddEquipment(QString name, bool enabled) {
        if (enabled) {
            if (!enabled_eq_state_.contains(name) && !enabled_eq_status_.contains(name)) {
                qDebug() << "EquipmentGroupState::AddEquipment Adding state: " << name;
                enabled_eq_state_[name] = EquipmentState::UNKNOWN;
                enabled_eq_status_[name] = EquipmentStatus::UNKNOWN;
            } else {
                qWarning() << "EquipmentGroupState::AddEquipment Equipment state already exists: " << name;
            }
        } else {
            if (!disabled_eq_state_.contains(name) && !disabled_eq_status_.contains(name)) {
                qDebug() << "EquipmentGroupState::AddEquipment Adding status: " << name;
                disabled_eq_state_[name] = EquipmentState::UNKNOWN;
                disabled_eq_status_[name] = EquipmentStatus::UNKNOWN;
            } else {
                qWarning() << "EquipmentGroupState::AddEquipment Equipment status already exists: " << name;
            }
        }
}

void EquipmentGroupState::ClearEquipments() {
    enabled_eq_state_.clear();
    enabled_eq_status_.clear();
    disabled_eq_state_.clear();
    disabled_eq_status_.clear();
    emit SIGNAL_Unknown();
}

void EquipmentGroupState::ChangeState(QString name, EquipmentState state) {
    if (enabled_eq_state_.contains(name)) {
        enabled_eq_state_[name] = state;
        CheckGroupState();
    } else if (disabled_eq_state_.contains(name)) {
        disabled_eq_state_[name] = state;
        CheckGroupState();
    } else {
        qWarning() << "EquipmentGroupState::ChangeState Equipment not found in group: " << name;
    }
}

void EquipmentGroupState::ChangeStatus(QString name, EquipmentStatus status) {
    if (enabled_eq_status_.contains(name)) {
        enabled_eq_status_[name] = status;
        CheckGroupState();
    } else if (disabled_eq_status_.contains(name)) {
        disabled_eq_status_[name] = status;
        CheckGroupState();
    } else {
        qWarning() << "EquipmentGroupState::ChangeStatus Equipment not found in group: " << name;
    }
}

void EquipmentGroupState::CheckGroupState() {
    if (IsOff()) { emit SIGNAL_Off(); }
    if (IsReady()) { emit SIGNAL_Ready(); }
    if (IsUnknown()) { emit SIGNAL_Unknown(); }
    if (IsIntermediate()) { emit SIGNAL_Intermediate(); }
}

bool EquipmentGroupState::IsOff() const {
    const bool all_enabled_off = (enabled_eq_state_.values().count(EquipmentState::OFF) == enabled_eq_state_.count());
    const bool all_disabled_off = (disabled_eq_state_.values().count(EquipmentState::OFF) == disabled_eq_state_.count());
    return (all_enabled_off && all_disabled_off);
}

bool EquipmentGroupState::IsReady() const {
    const bool all_enabled_on = (enabled_eq_state_.values().count(EquipmentState::ON) == enabled_eq_state_.count());
    const bool all_enabled_ontarget = (enabled_eq_status_.values().count(EquipmentStatus::ONTARGET) == enabled_eq_status_.count());
    const bool all_disabled_off = (disabled_eq_state_.values().count(EquipmentState::OFF) == disabled_eq_state_.count());
    return (all_enabled_on && all_enabled_ontarget && all_disabled_off);
}

bool EquipmentGroupState::IsUnknown() const {
    const bool any_enabled_unknown = enabled_eq_state_.values().contains(EquipmentState::UNKNOWN);
    const bool any_disabled_unknown = disabled_eq_state_.values().contains(EquipmentState::UNKNOWN);
    return (any_enabled_unknown || any_disabled_unknown);
}

bool EquipmentGroupState::IsIntermediate() const {
    return (!IsOff() && !IsReady() && !IsUnknown());
}

} // namespace
