#include "OmronSim.h"
#include "ui_OmronSim.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDebug>
#include <unistd.h>
#include <QtGlobal>
#include <QLabel>

namespace medicyc::cyclotroncontrolsystem::simulators::omron {

OmronSim::OmronSim(VirtualOmron *omron, QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::OmronSim),
    omron_(omron)
{
    ui_->setupUi(this);
    SetupLayout();
    setWindowTitle("OmronSim");

    SetupDatabase();
    auto channels = repo_->GetChannels("Source");
    omron_->SetChannels(channels);
    DisplayOmronChannels(channels);
    QObject::connect(ui_->pushButton_connect, &QPushButton::clicked, this, [&](bool connected) { omron_->SetConnected(connected); });

}

OmronSim::~OmronSim() {

}

void OmronSim::ReadRequest(QString name) {
    if (gridChannelMap_.contains(name)) {
        const bool content = gridChannelMap_.value(name)->text().toInt();
        FlashBackground(gridChannelMap_.value(name));
        emit SIGNAL_ReplyToReadRequest(name, content);
    } else {
        qWarning() << "OmronSim::ReadRequest Didnt find channel " << name;
    }
}

void OmronSim::WriteRequest(QString name, bool content) {
    if (gridChannelMap_.contains(name)) {
        gridChannelMap_.value(name)->setText(QString::number(static_cast<int>(content)));
        FlashBackground(gridChannelMap_.value(name));
        emit SIGNAL_ReplyToWriteRequest(name, content);
    } else {
        qWarning() << "OmronSim::WriteRequest Didnt find channel " << name;
    }
}

void OmronSim::SetupLayout() {
    QWidget *widget = new QWidget;
    QVBoxLayout *v_layout = new QVBoxLayout(widget);
    gridLayout_channels = new QGridLayout(widget);
    v_layout->addLayout(gridLayout_channels);

    widget->setLayout(v_layout);
    ui_->scrollArea->setWidget(widget);
}

void OmronSim::SetupDatabase() {
    qDebug() << "OmronSim::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_omron::OmronRepo>(new hw_omron::OmronRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "Omron", "Connection to database failed");
    }
}

void OmronSim::DisplayOmronChannels(const QMap<QString, hw_omron::Channel> channels) {
    qDebug() << "OmronSim::DisplayOmronChannels";
    const int NFIELDSPERCHANNEL = 9; // n items + spacer
    const int max_nrows = 20;
    const int ncols = 1 + static_cast<int>(channels.size()) / max_nrows;
    QFont header_font("LKLUG", 10, QFont::Bold);

    // Titles
    for (int icol = 0; icol < ncols; ++icol) {
        QLabel *lSystem = new QLabel("System");
        lSystem->setFixedHeight(30);
        gridLayout_channels->addWidget(lSystem,    0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::SYSTEM));
        gridLayout_channels->addWidget(new QLabel("Name"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel("Node"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NODE));
        gridLayout_channels->addWidget(new QLabel("Area"),      0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::AREA));
        gridLayout_channels->addWidget(new QLabel("Address"),   0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::ADDRESS));
        gridLayout_channels->addWidget(new QLabel("Bit"),       0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel("Impuls"),   0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::IMPULS));
        gridLayout_channels->addWidget(new QLabel("State"),     0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::STATE));

        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(60);
        gridLayout_channels->addWidget(lSpacer, 0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::STATE) + 1);

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::SYSTEM))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::NODE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::AREA))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::ADDRESS))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::BIT))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::IMPULS))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol*NFIELDSPERCHANNEL + static_cast<int>(COLUMNS::STATE))->widget())->setFont(header_font);
    }

    for (int iCh = 0; iCh < static_cast<int>(channels.values().size()); ++iCh) {
        int row = 1 + (iCh % max_nrows);
        int col = NFIELDSPERCHANNEL * (iCh / max_nrows);
        hw_omron::Channel ch = channels.values().at(iCh);
        gridLayout_channels->addWidget(new QLabel(ch.system()), row, col + static_cast<int>(COLUMNS::SYSTEM));
        gridLayout_channels->addWidget(new QLabel(ch.name()),   row, col + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.node())), row, col + static_cast<int>(COLUMNS::NODE));
        gridLayout_channels->addWidget(new QLabel(FromFinsMemoryArea(ch.fins_memeory_area())), row, col + static_cast<int>(COLUMNS::AREA));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.address())), row, col + static_cast<int>(COLUMNS::ADDRESS));
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.bit())), row, col + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel(ch.impuls() ? QString("T") : QString("F")), row, col + static_cast<int>(COLUMNS::IMPULS));

        QPushButton *lValue = new QPushButton("0");
        lValue->setFixedWidth(40);
        gridLayout_channels->addWidget(lValue, row, col + static_cast<int>(COLUMNS::STATE));
        QObject::connect(lValue, &QPushButton::clicked, this, [lValue]() {
            lValue->setText(lValue->text() == QString("0") ? QString("1") : QString("0")); });

        gridChannelMap_[ch.name()] = lValue;
    }
}

void OmronSim::FlashBackground(QPushButton *state) {
    state->setStyleSheet("QPushButton {background: #3daee9}");
    QTimer::singleShot(40, this, [state]() { state->setStyleSheet("QPushButton {background: solid black"); } );
}

QString OmronSim::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}


}
