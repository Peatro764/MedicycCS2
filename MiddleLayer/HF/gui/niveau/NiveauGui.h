#ifndef MIDDLELAYER_NIVEAU_HF_H
#define MIDDLELAYER_NIVEAU_HF_H

#include <QWidget>

#include "Niveau.h"

namespace Ui {
class NiveauGui;
}

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class NiveauGui : public QWidget
{
    Q_OBJECT

public:
    explicit NiveauGui(QWidget *parent = 0);
    ~NiveauGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

     Ui::NiveauGui *ui_;
     Niveau niveau_hf1_;
     Niveau niveau_hf2_;
};

}

#endif
