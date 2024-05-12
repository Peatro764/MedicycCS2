#include "IOLoadUI.h"

#include <QHBoxLayout>
#include <QLabel>

#include "Utils.h"
#include "PoleLevelIndicator.h"
#include "DBus.h"

#include "adc_interface.h"
#include "ior_interface.h"
#include "omron_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

IOLoadUI::IOLoadUI(QWidget *parent) :
    QWidget(parent) {

    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("Charge d'E/S", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);

    // Central rows (IO load markers)
    QGridLayout* grid_layout = SetupIOLoadMarkers();

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout);

    // Check dbus connection and grey out element if not active
    QTimer *timer_dbus = new QTimer();
    timer_dbus->setInterval(3000);
    timer_dbus->start();
    QObject::connect(timer_dbus, &QTimer::timeout, this, &IOLoadUI::SIGNAL_CheckDbusConnection);
}

QGridLayout* IOLoadUI::SetupIOLoadMarkers() {
    QHBoxLayout *layout_adc = new QHBoxLayout;
    QLabel *label_adc = shared::utils::GetLabel("ADC", shared::FONT::SMALL_LABEL);
    PoleLevelIndicator *pole_adc = new PoleLevelIndicator(this, 10, 25, 3, 8);
    layout_adc->addWidget(label_adc);
    layout_adc->addWidget(pole_adc);
    medicyc::cyclotron::ADCInterface *adc = new medicyc::cyclotron::ADCInterface("medicyc.cyclotron.hardware.adc", "/ADC", medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
    QObject::connect(adc, &medicyc::cyclotron::ADCInterface::SIGNAL_IOLoad, this, [&, pole_adc](double load) { pole_adc->SetLevel(load); });
    QObject::connect(this, &IOLoadUI::SIGNAL_CheckDbusConnection, this, [&, pole_adc, adc]() { pole_adc->SetEnabled(adc->isValid()); });

    QHBoxLayout *layout_ior = new QHBoxLayout;
    QLabel *label_ior = shared::utils::GetLabel("IOR", shared::FONT::SMALL_LABEL);
    PoleLevelIndicator *pole_ior = new PoleLevelIndicator(this, 10, 25, 3, 8);
    layout_ior->addWidget(label_ior);
    layout_ior->addWidget(pole_ior);
    medicyc::cyclotron::IORInterface *ior = new medicyc::cyclotron::IORInterface("medicyc.cyclotron.hardware.ior", "/IOR", medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
    QObject::connect(ior, &medicyc::cyclotron::IORInterface::SIGNAL_IOLoad, this, [&, pole_ior](double load) { pole_ior->SetLevel(load); });
    QObject::connect(this, &IOLoadUI::SIGNAL_CheckDbusConnection, this, [&, pole_ior, ior]() { pole_ior->SetEnabled(ior->isValid()); });

    auto SetupOmronPole = [&](QString name) -> QHBoxLayout* {
        QHBoxLayout *layout_omron = new QHBoxLayout;
        QLabel *label_omron = shared::utils::GetLabel(QString("OMRON ") + name, shared::FONT::SMALL_LABEL);
        PoleLevelIndicator *pole_omron = new PoleLevelIndicator(this, 10, 25, 3, 8);
        layout_omron->addWidget(label_omron);
        layout_omron->addWidget(pole_omron);
        medicyc::cyclotron::OmronInterface *omron = new medicyc::cyclotron::OmronInterface("medicyc.cyclotron.hardware.omron." + name.toLower(), "/Omron", medicyc::cyclotroncontrolsystem::global::GetDBusConnection());
        QObject::connect(omron, &medicyc::cyclotron::OmronInterface::SIGNAL_IOLoad, this, [&, pole_omron](double load) { pole_omron->SetLevel(load); });
        QObject::connect(this, &IOLoadUI::SIGNAL_CheckDbusConnection, this, [&, pole_omron, omron]() { pole_omron->SetEnabled(omron->isValid()); });
        return layout_omron;
    };
    auto layout_omron_cooling = SetupOmronPole("Cooling");
    auto layout_omron_source = SetupOmronPole("Source");
    auto layout_omron_hf = SetupOmronPole("HF");
    auto layout_omron_vdf = SetupOmronPole("VDF");

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addLayout(layout_adc, 0, 0, Qt::AlignHCenter);
    gridLayout->addLayout(layout_ior, 0, 1, Qt::AlignHCenter);
    gridLayout->addLayout(layout_omron_source, 0, 2, Qt::AlignHCenter);
    gridLayout->addLayout(layout_omron_hf, 0, 3, Qt::AlignHCenter);
    gridLayout->addLayout(layout_omron_vdf, 0, 4, Qt::AlignHCenter);
    gridLayout->addLayout(layout_omron_cooling, 0, 5, Qt::AlignHCenter);
    return gridLayout;
}

IOLoadUI::~IOLoadUI() {
}

}
