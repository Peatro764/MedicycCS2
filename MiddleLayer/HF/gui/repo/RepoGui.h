#ifndef MIDDLELAYER_REPOGUI_HF_H
#define MIDDLELAYER_REPOGUI_HF_H

#include <QWidget>

#include "HFRepo.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

class RepoGui : public QObject
{
    Q_OBJECT

public:
    explicit RepoGui();
    ~RepoGui();

public slots:

private slots:

signals:

private:
    void ConnectSignals();

    HFRepo repo_;
};

}

#endif
