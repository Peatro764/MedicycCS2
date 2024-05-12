#ifndef HFCONFIGURATIONUI_H
#define HFCONFIGURATIONUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>
#include <map>
#include <QLabel>

#include "Style.h"
#include "Utils.h"
#include "HFRepo.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace hf_middlelayer = medicyc::cyclotroncontrolsystem::middlelayer::hf;

class HFConfigurationUI : public QWidget
{
    Q_OBJECT
public:
    HFConfigurationUI(QWidget *parent = 0);
    ~HFConfigurationUI();

public slots:

signals:

private slots:
    void SaveParameters();

private:
    QGridLayout* SetupParameterGrid();
    hf_middlelayer::Configuration conf_hf1_;
    hf_middlelayer::Configuration conf_hf2_;
    hf_middlelayer::HFRepo repo_;
};

}

#endif
