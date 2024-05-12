#ifndef EQUIPMENTGROUPVALUES_H
#define EQUIPMENTGROUPVALUES_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::main {

class EquipmentValues {
public:
    EquipmentValues() {};
    EquipmentValues(QString state, QString des_value, QString act_value) {
        state_ = state;
        des_value_ = des_value;
        act_value_ = act_value;
    }
    QString toString() {
        QString str("State ");
        str.append(state_).append(" Des ").append(des_value_).append(" Act ").append(act_value_);
        return str;
    }
    QString state() const { return state_; }
    QString des_value() const { return des_value_; }
    QString act_value() const { return act_value_; }
private:
    QString state_;
    QString des_value_;
    QString act_value_;
};

class EquipmentGroupValues
{
public:
    EquipmentGroupValues(QString sub_system);
    ~EquipmentGroupValues();
    QString GetSubSystem() const { return sub_system_; }
    void AddEquipment(QString name, EquipmentValues values);
    QMap<QString, EquipmentValues> GetValues() const { return values_; }
    void ClearEquipments() { values_.clear(); }
    void print();

private:
    QString sub_system_;
    QMap<QString, EquipmentValues> values_;
};

} // namespace

#endif // EQUIPMENTGROUPVALUES_H
