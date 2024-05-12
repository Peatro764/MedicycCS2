#include "VacuumUI.h"

#include <QVBoxLayout>

#include "Utils.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

VacuumUI::VacuumUI(QWidget *parent) :
    QWidget(parent),
    interface_("medicyc.cyclotron.middlelayer.vacuumsystem", "/VacuumSystem", medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    repo_("_prod")
{
    if (!interface_.isValid()) {
        qDebug() << "DBUS connection error";
    }

    CreateChannels();

    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("Jauges Vide", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);

    // Central rows (jauge indicators)
    QGridLayout* grid_layout = SetupStatusIcons();

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout);

    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::VacuumSystemInterface::SIGNAL_ChannelState, this, &VacuumUI::UpdateChannelState);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::VacuumSystemInterface::SIGNAL_VacuumPartiallyOk, this, &VacuumUI::SetPartiallyOn);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::VacuumSystemInterface::SIGNAL_AllVacuumOk, this, &VacuumUI::SetAllOn);
    QObject::connect(&interface_, &medicyc::cyclotron::middlelayer::VacuumSystemInterface::SIGNAL_AllVacuumError, this, &VacuumUI::SetAllOff);
}

VacuumUI::~VacuumUI() {
    for (const auto& [name, widget] : gauges_) {
        delete widget;
    }
    gauges_.clear();
}

void VacuumUI::SetPartiallyOn() {
    all_ok_ = false;
    emit SIGNAL_State_Intermediate();
}

void VacuumUI::SetAllOn() {
    all_ok_ = true;
    emit SIGNAL_State_Ready();
}

void VacuumUI::SetAllOff() {
    all_ok_ = false;
    emit SIGNAL_State_Off();
}

QGridLayout* VacuumUI::SetupStatusIcons() {
    QGridLayout *gridLayout = new QGridLayout;
    int idx = 0;
    const int MAX_COL = 5;
    for (const auto& [name, widget] : gauges_) {
        QString label_text = name;
        label_text.replace("Q Eletta ", "");
        QLabel *l = shared::utils::GetLabel(label_text, shared::FONT::SMALL_LABEL);
        QHBoxLayout *h_layout = new QHBoxLayout;
        h_layout->addWidget(widget, Qt::AlignLeft);
        h_layout->addWidget(l, Qt::AlignLeft);
        gridLayout->addLayout(h_layout, idx / MAX_COL, idx % MAX_COL, Qt::AlignLeft);
        idx++;
    }
    return gridLayout;
}

void VacuumUI::CreateChannels() {
    try {
        auto channels = repo_.GetIORChannelNames("Vacuum");
        for (auto &c: channels) {
            qDebug() << "VacuumUI::CreateChannels " << c;
            gauges_[c] = new GenericDisplayButton(nullptr, 20);
            gauges_[c]->SetupAsColorChanging(Qt::green, Qt::red);
        }
    } catch (std::exception& exc) {
        qWarning() << "VacuumUI::CreateChannels Caught exception " << exc.what();
    }
}

void VacuumUI::UpdateChannelState(const QString& channel, bool state) {
//    qDebug() << "VacuumUI::UpdateChannelState " << channel << " " << state;
    if (auto search = gauges_.find(channel); search != gauges_.end()) {
        state ? gauges_[channel]->On() : gauges_[channel]->Off();
    } else {
        qWarning() << "VacuumUI::UpdateChannelState Unknown channel " << channel << " state " << state;
    }
}

void VacuumUI::Configure(QString name) {
    (void) name;
    // no actions
}

void VacuumUI::Startup() {
    // no actions
}

void VacuumUI::Shutdown() {
    // no actions
}

void VacuumUI::Interrupt() {
    // no actions
}


} // ns
