#ifndef HFSTATE_H
#define HFSTATE_H

#include <QObject>
#include <QMap>
#include <QTimer>

#include "HFUtils.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf {

class HFState : public QObject
{
    Q_OBJECT
public:
    explicit HFState(QObject *parent = nullptr);

public slots:
    void SetNiveauOn(HFX hf, bool on);
    void SetDeeVoltage(HFX hf, double voltage);
    void SetRegulation(HFX hf, bool on);
    void SetFrequencyWarm(bool state);
    void SetFrequencyCold(bool state);
    void SetPowerSuppliesAllOn(bool state);
    void SetPowerSuppliesAllOff(bool state);

signals:
    void SIGNAL_Unknown();
    void SIGNAL_Off();
    void SIGNAL_Intermediate();
    void SIGNAL_On();

private:
    void CheckState();
    QMap<HFX, bool> niveau_on_;
    QMap<HFX, bool> dee_warm_;
    QMap<HFX, bool> regulation_on_;
    bool frequency_cold_ = false;
    bool frequency_warm_ = false;
    bool powersupply_all_on_ = false;
    bool powersupply_all_off_ = false;
    const double DEE_VOLTAGE_WARM = 40; // kv
    QTimer timeout_; //
};


}

#endif // HFSTATE_H
