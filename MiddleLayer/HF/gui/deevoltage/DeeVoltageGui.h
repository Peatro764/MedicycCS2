#ifndef MIDDLELAYER_DEEVOLTAGE_H
#define MIDDLELAYER_DEEVOLTAGE_H

#include <QWidget>

#include "Dee.h"

namespace Ui {
class DeeVoltageGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class DeeVoltageGui : public QWidget
{
    Q_OBJECT

public:
    explicit DeeVoltageGui(QWidget *parent = 0);
    ~DeeVoltageGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::DeeVoltageGui *ui_;
     Dee dee_hf1_;
     Dee dee_hf2_;
};

}

#endif
