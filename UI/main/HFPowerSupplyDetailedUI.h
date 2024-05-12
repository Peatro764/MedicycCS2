#ifndef HFPOWERSUPPLYDETAILEDUI_H
#define HFPOWERSUPPLYDETAILEDUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
#include <map>
#include <QLabel>

#include "Style.h"
#include "GenericDisplayButton.h"
#include "hfpowersupplies_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class HFPowerSupplyDetailedUI : public QWidget
{
    Q_OBJECT
public:
    HFPowerSupplyDetailedUI(QWidget *parent = 0);
    ~HFPowerSupplyDetailedUI();
    void ReadDeviceStatus();

public slots:

signals:

private slots:
    void CreateDevices();
    QGridLayout* SetupStatusIcons();
    void UpdateDeviceState(const QString& channel, bool state);
    void UpdateDeviceDefaut(const QString& channel, bool active);
    void UpdateDeviceVoltage(const QString& channel, double value);

private:
    medicyc::cyclotron::HFPowerSuppliesInterface powersupplies_interface_;
    // TODO: Get these from the DB
    const QStringList devices_ = { "HF1 Grille PreAmpli", "HF1 Anode PreAmpli", "HF1 Ecran PreAmpli" ,
                             "HF1 Grille Ampli", "HF1 Ecran Ampli", "HF1 Filament PreAmpli",
                             "HF1 Filament Ampli",
                             "HF2 Grille PreAmpli", "HF2 Anode PreAmpli", "HF2 Ecran PreAmpli" ,
                             "HF2 Grille Ampli", "HF2 Ecran Ampli", "HF2 Filament PreAmpli",
                             "HF2 Filament Ampli",
                             "Alim THT", "Alim THT Aux" };
    std::map<QString, GenericDisplayButton*> device_states_;
    std::map<QString, GenericDisplayButton*> device_defauts_;
    std::map<QString, QLabel*> device_voltages_;
};

}

#endif
