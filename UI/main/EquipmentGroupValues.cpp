#include "EquipmentGroupValues.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

EquipmentGroupValues::EquipmentGroupValues(QString sub_system)
    : sub_system_(sub_system) {

}

EquipmentGroupValues::~EquipmentGroupValues() {}

void EquipmentGroupValues::AddEquipment(QString name, EquipmentValues values) {
    if (!values_.contains(name)) {
        values_[name] = values;
    } else {
        qWarning() << "EquipmentGroupValues::AddEquipment Already exists in map: " << name;
    }
}

void EquipmentGroupValues::print() {
    for (auto k : values_.keys()) {
        qDebug() << k << " : " << values_[k].toString();
    }
}

} // namespace
