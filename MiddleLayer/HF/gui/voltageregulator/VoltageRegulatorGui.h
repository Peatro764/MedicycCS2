#ifndef MIDDLELAYER_VOLTAGEREGULATORGUI_HF_H
#define MIDDLELAYER_VOLTAGEREGULATORGUI_HF_H

#include <QWidget>

#include "VoltageRegulator.h"

namespace Ui {
class VoltageRegulatorGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class VoltageRegulatorGui : public QWidget
{
    Q_OBJECT

public:
    explicit VoltageRegulatorGui(QWidget *parent = 0);
    ~VoltageRegulatorGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::VoltageRegulatorGui *ui_;
     VoltageRegulator *regulator_hf1_;
     VoltageRegulator *regulator_hf2_;
     Niveau niveau_hf1_;
     Niveau niveau_hf2_;
     Dee dee_hf1_;
     Dee dee_hf2_;
};

}

#endif
