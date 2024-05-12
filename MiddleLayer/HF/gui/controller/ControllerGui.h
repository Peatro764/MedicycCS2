#ifndef MIDDLELAYER_HF_CONTROLLERGUI_H
#define MIDDLELAYER_HF_CONTROLLERGUI_H

#include <QWidget>

#include "HFController.h"

namespace Ui {
class ControllerGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class ControllerGui : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerGui(QWidget *parent = 0);
    ~ControllerGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::ControllerGui *ui_;
     HFController controller_;
};

}

#endif
