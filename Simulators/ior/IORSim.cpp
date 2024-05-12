#include "IORSim.h"
#include "ui_IORSim.h"

#include <QStandardPaths>
#include <QMessageBox>
#include <QPushButton>

#include "Parameters.h"

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

namespace medicyc::cyclotroncontrolsystem::simulators::ior {

IORSim::IORSim(VirtualIOR *ior, QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::IORSim),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
    ior_(ior)
{
    ui_->setupUi(this);
    setWindowTitle("IORSim");
    SetupLayout();
    SetupDatabase();
    auto channels = GetIORChannels();
    ior_->SetChannels(channels);
    DisplayUIChannels(channels);
}

IORSim::~IORSim() {

}

void IORSim::ReadValueRequest(QString name) {
    if (!ior_->ChannelExist(name)) {
        qWarning() << "IORSim::ReadValueRequest Channel does not exist " << name;
        return;
    }

    hw_ior::Channel channel = ior_->GetChannel(name);
    if (channel_value_button_.contains(name)) {
        QPushButton *b = channel_value_button_.value(name);
        FlashBackground(b);
        emit SIGNAL_ReplyToReadValueRequest(name, b->text() == QString("1"));
    }
}

void IORSim::ReadMaskRequest(QString name) {
    if (!ior_->ChannelExist(name)) {
        qWarning() << "IORSim::ReadMaskRequest Channel does not exist " << name;
        return;
    }

    hw_ior::Channel channel = ior_->GetChannel(name);
    if (channel_mask_button_.contains(name)) {
        QPushButton *b = channel_mask_button_.value(name);
        FlashBackground(b);
        emit SIGNAL_ReplyToReadMaskRequest(name, b->text() == QString("1"));
    }
}

void IORSim::SetupLayout() {
    QWidget *widget = new QWidget;
    QVBoxLayout *v_layout = new QVBoxLayout(widget);
    QHBoxLayout *h_layout = new QHBoxLayout(widget);
    QLabel *l1 = new QLabel("CARD 1");
    l1->setFont(QFont("Ubuntu", 18));
    l1->setAlignment(Qt::AlignCenter);
    QLabel *l2 = new QLabel("CARD 2");
    l2->setFont(QFont("Ubuntu", 18));
    l2->setAlignment(Qt::AlignCenter);
    QLabel *l3 = new QLabel("CARD 3");
    l3->setFont(QFont("Ubuntu", 18));
    l3->setAlignment(Qt::AlignCenter);
    QLabel *l4 = new QLabel("CARD 4");
    l4->setFont(QFont("Ubuntu", 18));
    l4->setAlignment(Qt::AlignCenter);
    h_layout->addWidget(l1);
    h_layout->addWidget(l2);
    h_layout->addWidget(l3);
    h_layout->addWidget(l4);

    gridLayout_channels = new QGridLayout(widget);
    v_layout->addLayout(h_layout);
    v_layout->addLayout(gridLayout_channels);

    widget->setLayout(v_layout);
    ui_->scrollArea->setWidget(widget);
}

void IORSim::SetupDatabase() {
    qDebug() << "IORSim::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_ior::IORRepo>(new hw_ior::IORRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "IOR", "Connection to database failed");
    }
}

std::vector<hw_ior::Channel>  IORSim::GetIORChannels() {
    qDebug() << "IORSim::GetIORChannels";
    std::vector<hw_ior::Channel> channels;
    try {
        QStringList names = repo_->GetIORChannelNames();
        for (QString name : names) {
            channels.push_back(repo_->GetIORChannel(name));
            qDebug() << "IORSim::GetIORChannels Found channel: " << name;
        }
    }
    catch (std::exception& exc) {
//        ui_->messagePlainTextEdit->appendPlainText(exc.what());
    }
    return channels;
}

void IORSim::DisplayUIChannels(const std::vector<hw_ior::Channel>& channels) {
    qDebug() << "IORSim::DisplayUIChannels";
    QFont header_font("LKLUG", 10, QFont::Bold);

    for (int iCard = 0; iCard < NCARDS; ++iCard) {
        gridLayout_channels->addWidget(new QLabel("Ch"), 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::CHANNEL));
        gridLayout_channels->addWidget(new QLabel("Bit"), 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel("Name"), 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel("State"), 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::VALUE));
        gridLayout_channels->addWidget(new QLabel("Mask"), 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::MASK));
        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(60);
        gridLayout_channels->addWidget(lSpacer, 0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::SPACER));

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::CHANNEL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::BIT))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::VALUE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * NCOLSPERBIT + static_cast<int>(COLUMNS::MASK))->widget())->setFont(header_font);
    }
    for (hw_ior::Channel ch : channels) {
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Block())),   1 + NBITSPERBLOCK*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::CHANNEL) + NCOLSPERBIT * ch.Card());
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Address())), 1 + NBITSPERBLOCK*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::BIT) + NCOLSPERBIT * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Name()),                     1 + NBITSPERBLOCK*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::NAME) + NCOLSPERBIT * ch.Card());
        QPushButton *bValue = new QPushButton("0");
        gridLayout_channels->addWidget(bValue, 1 + NBITSPERBLOCK*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::VALUE) + NCOLSPERBIT * ch.Card());
        QObject::connect(bValue, &QPushButton::clicked, this, [&, bValue]() {bValue->setText(bValue->text() == QString("0") ? "1" : "0"); });
        QPushButton *bMask = new QPushButton("0");
        gridLayout_channels->addWidget(bMask, 1 + NBITSPERBLOCK*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::MASK) + NCOLSPERBIT * ch.Card());

        channel_value_button_[ch.Name()] = bValue;
        channel_mask_button_[ch.Name()] = bMask;
     }
}

void IORSim::FlashBackground(QPushButton *button) {
    button->setStyleSheet("QPushButton {background: #3daee9}");
    QTimer::singleShot(40, this, [button]() { button->setStyleSheet("QPushButton {background: solid black"); } );
}

QString IORSim::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

void IORSim::ShowConnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void IORSim::ShowDisconnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}

}
