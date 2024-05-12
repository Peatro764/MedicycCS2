#ifndef EQUIPMENTGROUPSTATE_H
#define EQUIPMENTGROUPSTATE_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::middlelayer::utils {

enum class EquipmentState {
    UNKNOWN = -1,
    OFF = 0,
    ON = 1
};

enum class EquipmentStatus {
    UNKNOWN = 0,
    OFFTARGET = 1,
    ONTARGET = 2
};

class EquipmentGroupState : public QObject
{
    Q_OBJECT
public:
    EquipmentGroupState();
    ~EquipmentGroupState();
    void AddEquipment(QString name, bool enabled);
    void ClearEquipments();
    void ChangeState(QString name, EquipmentState state);
    void ChangeStatus(QString name, EquipmentStatus status);

signals:
    void SIGNAL_Unknown();
    void SIGNAL_Intermediate();
    void SIGNAL_Off();
    void SIGNAL_Ready();

private:
    void CheckGroupState();
    bool IsOff() const;
    bool IsReady() const;
    bool IsUnknown() const;
    bool IsIntermediate() const;

    QMap<QString, EquipmentState> enabled_eq_state_;
    QMap<QString, EquipmentStatus> enabled_eq_status_;
    QMap<QString, EquipmentState> disabled_eq_state_;
    QMap<QString, EquipmentStatus> disabled_eq_status_;
};

} // namespace

#endif // EQUIPMENTGROUPSTATE_H
