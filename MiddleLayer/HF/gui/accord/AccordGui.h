#ifndef MIDDLELAYER_ACCORDGUI_HF_H
#define MIDDLELAYER_ACCORDGUI_HF_H

#include <QWidget>

#include "Accord.h"

namespace Ui {
class AccordGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class AccordGui : public QWidget
{
    Q_OBJECT

public:
    explicit AccordGui(QWidget *parent = 0);
    ~AccordGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::AccordGui *ui_;
     Accord *accord_hf1_;
     Accord *accord_hf2_;
     Membrane membrane_hf1_;
     Membrane membrane_hf2_;
     Niveau niveau_hf1_;
     Niveau niveau_hf2_;
     Dee dee_hf1_;
     Dee dee_hf2_;
};

}

#endif
