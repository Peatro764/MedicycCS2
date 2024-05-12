#ifndef MIDDLELAYER_MEMBRANEGUI_H
#define MIDDLELAYER_MEMBRANEGUI_H

#include <QWidget>

#include "Membrane.h"

namespace Ui {
class MembraneGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class MembraneGui : public QWidget
{
    Q_OBJECT

public:
    explicit MembraneGui(QWidget *parent = 0);
    ~MembraneGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::MembraneGui *ui_;
     Membrane membrane_hf1_;
     Membrane membrane_hf2_;
};

}

#endif
