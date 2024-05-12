#include "IORGui.h"
#include "ui_IORGui.h"

#include <QStandardPaths>
#include <QMessageBox>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::ui::ior {

IORGui::IORGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::IORGui),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
    ior_()
{
    ui_->setupUi(this);
    SetupLayout();
    setWindowTitle("IOR");
    SetupDatabase();
    auto channels = GetIORChannels();
    ior_.SetChannels(channels);
    DisplayUIChannels(channels);
    ior_.ReadConfiguration();

    assert(QObject::connect(ui_->setCardAddressPushButton, SIGNAL(clicked()), this, SLOT(SetCardAddresses())));
    assert(QObject::connect(ui_->resetPushButton, SIGNAL(clicked()), &ior_, SLOT(Reset())));
    assert(QObject::connect(ui_->readConfigurationPushButton, SIGNAL(clicked()), &ior_, SLOT(ReadConfiguration())));
    assert(QObject::connect(ui_->enableFullScanPushButton, SIGNAL(clicked()), &ior_, SLOT(DoCompleteCyclicScans())));
    assert(QObject::connect(ui_->enablePartialScanPushButton, SIGNAL(clicked()), this, SLOT(SetPartialScan())));
    assert(QObject::connect(ui_->startScanPushButton, SIGNAL(clicked()), &ior_, SLOT(StartCyclicScan())));
    assert(QObject::connect(ui_->stopScanPushButton, SIGNAL(clicked()), &ior_, SLOT(StopCyclicScan())));
    assert(QObject::connect(ui_->pushButton_bitFormat, SIGNAL(clicked()), &ior_, SLOT(SetChannelTypeLAM())));
    assert(QObject::connect(ui_->pushButton_channelFormat, SIGNAL(clicked()), &ior_, SLOT(SetBlockTypeLAM())));
    assert(QObject::connect(ui_->stopScanPushButton, SIGNAL(clicked()), &ior_, SLOT(StopCyclicScan())));

    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_Connected()), this, SLOT(ShowConnected())));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_Disconnected()), this, SLOT(ShowDisconnected())));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_ReplyReceived()) , this, SLOT(ShowPing())));

    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_IsConfigured(bool)), ui_->isConfiguredRadioButton, SLOT(setChecked(bool))));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_ScanEnabled(bool)), ui_->isScansEnabledRadioButton, SLOT(setChecked(bool))));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_FullScan(bool)), ui_->isCompleteScansRadioButton, SLOT(setChecked(bool))));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_IsFormatChannel(bool)), ui_->isFormatChannelRadioButton, SLOT(setChecked(bool))));

    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_ReceivedChannelValue(QString, bool)), this, SLOT(ProcessChannelValue(QString, bool))));
    assert(QObject::connect(&ior_, SIGNAL(SIGNAL_ReceivedChannelMask(QString, bool)), this, SLOT(ProcessChannelMask(QString, bool))));
}

IORGui::~IORGui()
{
}

void IORGui::SetupLayout() {
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

void IORGui::SetupDatabase() {
    qDebug() << "IORGui::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_ior::IORRepo>(new hw_ior::IORRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "IOR", "Connection to database failed");
    }
}

std::vector<hw_ior::Channel>  IORGui::GetIORChannels() {
    qDebug() << "IORGui::GetIORChannels";
    std::vector<hw_ior::Channel> channels;
    try {
        QStringList names = repo_->GetIORChannelNames();
        for (QString name : names) {
            channels.push_back(repo_->GetIORChannel(name));
            qDebug() << "IORGui::GetIORChannels Found channel: " << name;
        }
    }
    catch (std::exception& exc) {
//        ui_->messagePlainTextEdit->appendPlainText(exc.what());
    }
    return channels;
}

void IORGui::DisplayUIChannels(const std::vector<hw_ior::Channel>& channels) {
    qDebug() << "IORGui::DisplayUIChannels";
    QFont header_font("LKLUG", 10, QFont::Bold);

    for (int iCard = 0; iCard < ior_.ncards(); ++iCard) {
        gridLayout_channels->addWidget(new QLabel("Ch"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::CHANNEL));
        gridLayout_channels->addWidget(new QLabel("Bit"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::BIT));
        gridLayout_channels->addWidget(new QLabel("Name"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::NAME));
        gridLayout_channels->addWidget(new QLabel("State"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::VALUE));
        gridLayout_channels->addWidget(new QLabel("Read"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::READ_VALUE));
        gridLayout_channels->addWidget(new QLabel("Mask"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::MASK));
        gridLayout_channels->addWidget(new QLabel("Read"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::READ_MASK));
        gridLayout_channels->addWidget(new QLabel("Write"), 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::WRITE_MASK));
        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(60);
        gridLayout_channels->addWidget(lSpacer, 0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::SPACER));

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::CHANNEL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::BIT))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::VALUE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::READ_VALUE))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::MASK))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::READ_MASK))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, iCard * ior_.ncolsperbit() + static_cast<int>(COLUMNS::WRITE_MASK))->widget())->setFont(header_font);
    }
    for (hw_ior::Channel ch : channels) {
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Block())),   1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::CHANNEL) + ior_.ncolsperbit() * ch.Card());
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Address())), 1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::BIT) + ior_.ncolsperbit() * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Name()),                     1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::NAME) + ior_.ncolsperbit() * ch.Card());
        QLabel *lValue = new QLabel("0");
        lValue->setAlignment(Qt::AlignCenter);
        gridLayout_channels->addWidget(lValue,                           1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::VALUE) + ior_.ncolsperbit() * ch.Card());
        QPushButton *bReadValue = new QPushButton("R");
        bReadValue->setFixedWidth(40);
        gridLayout_channels->addWidget(bReadValue,                                1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::READ_VALUE) + ior_.ncolsperbit() * ch.Card());
        QLineEdit *lMask = new QLineEdit("0");
        lMask->setAlignment(Qt::AlignCenter);
        gridLayout_channels->addWidget(lMask,                           1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::MASK) + ior_.ncolsperbit() * ch.Card());
        QPushButton *bReadMask = new QPushButton("R");
        bReadMask->setFixedWidth(40);
        gridLayout_channels->addWidget(bReadMask,                                 1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::READ_MASK) + ior_.ncolsperbit() * ch.Card());
        QPushButton *bWriteMask = new QPushButton("W");
        bWriteMask->setFixedWidth(40);
        gridLayout_channels->addWidget(bWriteMask,                                1 + ior_.nbitsperblock()*ch.Block() + ch.Address(), static_cast<int>(COLUMNS::WRITE_MASK) + ior_.ncolsperbit() * ch.Card());

        channel_value_label_[ch.Name()] = lValue;
        channel_mask_label_[ch.Name()] = lMask;

        QObject::connect(bReadValue, &QPushButton::clicked, this, [this, ch]() { ior_.ReadChannelValue(ch); });
        QObject::connect(bReadMask, &QPushButton::clicked, this, [this, ch]() { ior_.ReadBlockMask(ch.Card(), ch.Block()); });
        QObject::connect(bWriteMask, &QPushButton::clicked, this, [this, ch]() { WriteBlockMask(ch.Card(), ch.Block()); });
     }
}

void IORGui::SetCardAddresses() {
    const int address1(ui_->cardAdress1LineEdit->text().toInt());
    const int address2(ui_->cardAdress2LineEdit->text().toInt());
    ior_.SetCardAddressRange(address1, address2);
}

void IORGui::SetPartialScan() {
    const int address1(ui_->partielScanAddress1LineEdit->text().toInt());
    const int address2(ui_->partielScanAddress2LineEdit->text().toInt());
    ior_.DoPartialCyclicScans(address1, address2);
}

void IORGui::WriteBlockMask(int card, int block) {
    try {
        ior_.WriteBlockMask(card, block, GetBlockMask(card, block));
    }
    catch (std::exception& exc) {
        qDebug() << "IORGUi::WriteBlockMask Exception " << exc.what();
    }
}

quint8 IORGui::GetBlockMask(int card, int block) const {
    bool conv_ok(false);
    quint8 block_mask(0);
    try {
        for (int address = 0; address < ior_.nbitsperblock(); ++address) {
            QLayoutItem* mask_item = gridLayout_channels->itemAtPosition(1 + (ior_.nbitsperblock() * block) + address, static_cast<int>(COLUMNS::MASK) + ior_.ncolsperbit() * card);
            if (mask_item) {
                QWidget* widget = mask_item->widget();
                QLineEdit* label = dynamic_cast<QLineEdit*>(widget);
                quint8 bit_mask = static_cast<quint8>(label->text().toInt(&conv_ok));
                if (!conv_ok) { throw std::runtime_error("Mask value corrupt"); }
                block_mask += (bit_mask << address);
            }
        }
    }
    catch (std::exception& exc) {
        qDebug() << "IOGui::GetBlockMask Exception thrown: " << QString::fromStdString(exc.what());
    }
    return block_mask;
}

void IORGui::ProcessChannelValue(QString channel, bool value) {
    if (!channel_value_label_.contains(channel)) {
        qDebug() << "IORGui::ProcessChannelValue Map does not contain channel: " << channel;
    }
    QLabel *label = channel_value_label_.value(channel);
    FlashBackground(label);
    label->setText(QString::number(value));
}

void IORGui::ProcessChannelMask(QString channel, bool value) {
    if (!channel_mask_label_.contains(channel)) {
        qDebug() << "IORGui::ProcessChannelMask Map does not contain channel: " << channel;
    }
    QLineEdit *lineEdit = channel_mask_label_.value(channel);
    FlashBackground(lineEdit);
    lineEdit->setText(QString::number(value));
}

void IORGui::FlashBackground(QLabel *label) {
    label->setStyleSheet("QLabel {background: #3daee9}");
    QTimer::singleShot(40, this, [label]() { label->setStyleSheet("QLabel {background: solid black"); } );
}

void IORGui::FlashBackground(QLineEdit *lineEdit) {
    lineEdit->setStyleSheet("QLineEdit {background: #3daee9}");
    QTimer::singleShot(40, this, [lineEdit]() { lineEdit->setStyleSheet("QLineEdit {background: solid black"); } );
}

QString IORGui::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

void IORGui::ShowPing() {
    ui_->frame_ping->setStyleSheet(FrameStyleSheet("frame_ping", "circle-blue-20x20"));
    QTimer::singleShot(500, this, [&]() { ui_->frame_ping->setStyleSheet("QFrame {}"); });
}

void IORGui::ShowConnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void IORGui::ShowDisconnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}

}
