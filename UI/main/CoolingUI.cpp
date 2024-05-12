#include "CoolingUI.h"

#include <QVBoxLayout>

#include "Utils.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

CoolingUI::CoolingUI(QWidget *parent) :
    QWidget(parent),
    interface_("medicyc.cyclotron.middlelayer.coolingsystem", "/CoolingSystem", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    repo_("_prod")
{
    if (!interface_.isValid()) {
        qDebug() << "DBUS connection error";
    }

    CreateValves();

    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("Vannes Eletta", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);

    // Central rows (valve indicators)
    QGridLayout* grid_layout = SetupStatusIcons();

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout);

    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::CoolingSystemInterface::SIGNAL_ChannelState, this, &CoolingUI::UpdateChannelState);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::CoolingSystemInterface::SIGNAL_PartiallyOn, this, &CoolingUI::SetPartiallyOn);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::CoolingSystemInterface::SIGNAL_AllCoolingOn, this, &CoolingUI::SetAllOn);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::CoolingSystemInterface::SIGNAL_AllCoolingOff, this, &CoolingUI::SetAllOff);
}

CoolingUI::~CoolingUI() {
    for (const auto& [name, widget] : valves_) {
        delete widget;
    }
    valves_.clear();
}

void CoolingUI::SetPartiallyOn() {
    all_on_ = false;
    emit SIGNAL_State_Intermediate();
}

void CoolingUI::SetAllOn() {
    all_on_ = true;
    emit SIGNAL_State_Ready();
}

void CoolingUI::SetAllOff() {
    all_on_ = false;
    emit SIGNAL_State_Off();
}

QGridLayout* CoolingUI::SetupStatusIcons() {
    QGridLayout *gridLayout = new QGridLayout;
    int idx = 0;
    const int MAX_COL = 5;
    for (const auto& [name, widget] : valves_) {
        QString label_text = name;
        label_text.replace("Q Eletta ", "");
        QLabel *l = shared::utils::GetLabel(label_text, shared::FONT::SMALL_LABEL);
        QHBoxLayout *h_layout = new QHBoxLayout;
        h_layout->addWidget(widget, Qt::AlignLeft);
        h_layout->addWidget(l, Qt::AlignLeft);
        gridLayout->addLayout(h_layout, idx % MAX_COL, idx / MAX_COL, Qt::AlignLeft);
        idx++;
    }
    return gridLayout;
}

void CoolingUI::CreateValves() {
    try {
        auto channels = repo_.GetChannels("Cooling");
        for (auto &c: channels.keys()) {
            qDebug() << "CoolingUI::CreateValves " << c;
            valves_[c] = new GenericDisplayButton(nullptr, 20);
            valves_[c]->SetupAsColorChanging(Qt::green, Qt::red);
        }
    } catch (std::exception& exc) {
        qWarning() << "CoolingUI::CreateValves Caught exception " << exc.what();
    }
}

void CoolingUI::UpdateChannelState(const QString& channel, bool state) {
//    qDebug() << "CoolingUI::UpdateChannelState " << channel << " " << state;
    if (auto search = valves_.find(channel); search != valves_.end()) {
        state ? valves_[channel]->On() : valves_[channel]->Off();
    } else {
        qWarning() << "CoolingUI::UpdateChannelState Unknown channel " << channel << " state " << state;
    }
}

void CoolingUI::Configure(QString name) {
    (void) name;
    // no actions
}

void CoolingUI::Startup() {
    // no actions
}

void CoolingUI::Shutdown() {
    // no actions
}

void CoolingUI::Interrupt() {
    // no actions
}


}
