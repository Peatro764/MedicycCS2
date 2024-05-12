#ifndef MIDDLELAYER_HF_H
#define MIDDLELAYER_HF_H

#include <QWidget>

#include "HFPowerSupplies.h"

namespace Ui {
class HFGui;
}

namespace medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies {

class HFGui : public QWidget
{
    Q_OBJECT

public:
    explicit HFGui(QWidget *parent = 0);
    ~HFGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::HFGui *ui_;
     HFPowerSupplies power_supplies_;
};

}

#endif
