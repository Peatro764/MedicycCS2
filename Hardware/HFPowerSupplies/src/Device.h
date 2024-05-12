#ifndef MIDDLELAYER_DEVICE_H
#define MIDDLELAYER_DEVICE_H

#include <QObject>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

class Device : public QObject
{
    Q_OBJECT

public:

    enum class STATE { UNKNOWN, OFF, ON };

    Device(QString name, QString cmd_is_on, QString cmd_is_off, QString cmd_defaut_active, QString cmd_voltage);
    QString Name() const { return name_; }
    void SetVoltage(double value);
    void SetOn(bool state);
    void SetOff(bool state);
    void SetDefaut(bool state);
    bool IsDefaut() const { return defaut_; }
    QString GetCmdIsOn() const { return cmd_is_on_; }
    QString GetCmdIsOff() const { return cmd_is_off_; }
    QString GetCmdDefautActive() const { return cmd_defaut_active_; }
    QString GetCmdVoltage() const { return cmd_voltage_; }

signals:
    void SIGNAL_On();
    void SIGNAL_Off();
    void SIGNAL_Defaut_Active();
    void SIGNAL_No_Defaut();
    void SIGNAL_Voltage(double value);

private:
    void EvaluateState();
    QString name_;
    double voltage_ = 0.0;
    bool on_ = false;
    bool off_ = false;
    bool defaut_ = false;
    QString cmd_is_on_;
    QString cmd_is_off_;
    QString cmd_defaut_active_;
    QString cmd_voltage_;
};

}

#endif // DEVICE_H
