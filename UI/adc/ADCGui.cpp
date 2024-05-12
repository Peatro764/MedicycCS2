#include "ADCGui.h"
#include "ui_ADCGui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QMessageBox>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::ui::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

ADCGui::ADCGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::ADCGui),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
    adc_()
{
    ui_->setupUi(this);
    setWindowTitle("ADC");
    SetupLayout();
    SetupDatabase();
    auto channels = GetADCChannels();
    SetupPingTimers(channels);
    adc_.SetChannels(channels);
    DisplayUIChannels(channels);
    adc_.ReadConfiguration();

    //QObject::connect(&adc_, SIGNAL(SIGNAL_IOError(QString)), ui_->messagePlainTextEdit, SLOT(appendPlainText(QString)));

    QObject::connect(ui_->setCardAddressPushButton, &QPushButton::clicked, this, &ADCGui::SetCardAddresses);
    QObject::connect(ui_->resetPushButton, &QPushButton::clicked, &adc_, &hw_adc::ADC::Reset);
    QObject::connect(ui_->readConfigurationPushButton, &QPushButton::clicked, &adc_, &hw_adc::ADC::ReadConfiguration);
    QObject::connect(ui_->enableFullScanPushButton, &QPushButton::clicked, &adc_, &hw_adc::ADC::DoCompleteCyclicScans);
    QObject::connect(ui_->enablePartialScanPushButton, &QPushButton::clicked, this, &ADCGui::SetPartialScan);
    QObject::connect(ui_->startScanPushButton, &QPushButton::clicked, &adc_, &hw_adc::ADC::StartCyclicScan);
    QObject::connect(ui_->stopScanPushButton, &QPushButton::clicked, &adc_, &hw_adc::ADC::StopCyclicScan);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_Connected, this, &ADCGui::ShowConnected);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_Disconnected, this, &ADCGui::ShowDisconnected);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_ReplyReceived, this, &ADCGui::ShowPing);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_IsConfigured, ui_->isConfiguredRadioButton, &QRadioButton::setChecked);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_ScanEnabled, ui_->isScansEnabledRadioButton, &QRadioButton::setChecked);
    QObject::connect(&adc_, &hw_adc::ADC::SIGNAL_FullScan, ui_->isCompleteScansRadioButton, &QRadioButton::setChecked);
    QObject::connect(&adc_, qOverload<hw_adc::Channel, int>(&hw_adc::ADC::SIGNAL_ReceivedChannelValue), this, &ADCGui::ProcessChannelValue);
}

ADCGui::~ADCGui()
{
}

void ADCGui::SetupPingTimers(std::vector<hw_adc::Channel> channels) {
    for (const hw_adc::Channel& ch : channels) {
        QTimer *t = new QTimer();
        t->setInterval(1000);
        t->setSingleShot(false);
        QObject::connect(t, &QTimer::timeout, this, [this, ch]() { adc_.Read(ch); });
        ping_timers_[ch.Name()] = t;
    }
}

void ADCGui::SetupLayout() {
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
    l4->setAlignment(Qt::AlignHCenter);
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

void ADCGui::SetupDatabase() {
    qDebug() << "ADCGui::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_adc::ADCRepo>(new hw_adc::ADCRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "ADC", "Connection to database failed");
    }
}

std::vector<hw_adc::Channel>  ADCGui::GetADCChannels() {
    qDebug() << "ADCGui::GetADCChannels";
    std::vector<hw_adc::Channel> channels;
    try {
        QStringList names = repo_->GetADCChannelNames();
        for (QString& name : names) {
            channels.push_back(repo_->GetADCChannel(name));
            qDebug() << "ADCGui::GetADCChannels Found channel: " << name;
        }
    }
    catch (std::exception& exc) {
        //ui_->messagePlainTextEdit->appendPlainText(exc.what());
    }
    return channels;
}

void ADCGui::DisplayUIChannels(const std::vector<hw_adc::Channel>& channels) {
    qDebug() << "ADCGui::DisplayUIChannels";
    QFont header_font("LKLUG", 10, QFont::Bold);

    for (int icol = 0; icol < 4; ++icol) {
        gridLayout_channels->addWidget(new QLabel("Ch"), 0, icol * NCOL + static_cast<int>(ADC_COL::CHANNEL));
        gridLayout_channels->addWidget(new QLabel("Name"), 0, icol * NCOL + static_cast<int>(ADC_COL::NAME));
        gridLayout_channels->addWidget(new QLabel("Hex"), 0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_BINARY));
        gridLayout_channels->addWidget(new QLabel("Voltage"), 0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_ADC));
        gridLayout_channels->addWidget(new QLabel("Physical"), 0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_PHYSICAL));
        gridLayout_channels->addWidget(new QLabel("Unit"), 0, icol * NCOL + static_cast<int>(ADC_COL::UNIT_PHYSICAL));
        gridLayout_channels->addWidget(new QLabel("Read"), 0, icol * NCOL + static_cast<int>(ADC_COL::READ_BUTTON));
        gridLayout_channels->addWidget(new QLabel("Ping"), 0, icol * NCOL + static_cast<int>(ADC_COL::AUTO_BUTTON));
        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(40);
        gridLayout_channels->addWidget(lSpacer, 0, icol * NCOL + static_cast<int>(ADC_COL::SPACER));

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::CHANNEL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_BINARY))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_ADC))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_PHYSICAL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::UNIT_PHYSICAL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::READ_BUTTON))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::AUTO_BUTTON))->widget())->setFont(header_font);
    }

    for (const hw_adc::Channel& ch : channels) {
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Address())), 1 + ch.Address(), static_cast<int>(ADC_COL::CHANNEL) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Name()), 1 + ch.Address(), static_cast<int>(ADC_COL::NAME) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel("0"), 1 + ch.Address(), static_cast<int>(ADC_COL::VALUE_BINARY) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel("0"), 1 + ch.Address(), static_cast<int>(ADC_COL::VALUE_ADC) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel("0.000e0"), 1 + ch.Address(), static_cast<int>(ADC_COL::VALUE_PHYSICAL) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Unit()), 1 + ch.Address(), static_cast<int>(ADC_COL::UNIT_PHYSICAL) + NCOL * ch.Card());
        QPushButton *button_read = new QPushButton("R");
        gridLayout_channels->addWidget(button_read, 1 + ch.Address(), static_cast<int>(ADC_COL::READ_BUTTON) + NCOL * ch.Card());
        QObject::connect(button_read, &QPushButton::clicked, this, [this, ch]() { adc_.Read(ch); });
        QPushButton *button_auto = new QPushButton("P");
        button_auto->setCheckable(true);
        button_auto->setChecked(false);
        gridLayout_channels->addWidget(button_auto, 1 + ch.Address(), static_cast<int>(ADC_COL::AUTO_BUTTON) + NCOL * ch.Card());
        QObject::connect(button_auto, &QPushButton::clicked, this, [this, ch]() {
            QTimer *t = ping_timers_.value(ch.Name()); if (t->isActive()) t->stop(); else t->start();
        });
     }
}

void ADCGui::ReadChannels(const std::vector<hw_adc::Channel>& channels) {
    for (const hw_adc::Channel& ch : channels) {
        adc_.Read(ch);
    }
}

void ADCGui::SetCardAddresses() {
    const int address1(ui_->cardAdress1LineEdit->text().toInt());
    const int address2(ui_->cardAdress2LineEdit->text().toInt());
    adc_.SetCardAddressRange(address1, address2);
}

void ADCGui::SetPartialScan() {
    const int address1(ui_->partielScanAddress1LineEdit->text().toInt());
    const int address2(ui_->partielScanAddress2LineEdit->text().toInt());
    adc_.DoPartialCyclicScans(address1, address2);
}

void ADCGui::ProcessChannelValue(hw_adc::Channel channel, int value) {
    QLayoutItem* item_name = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::NAME) + NCOL * channel.Card());
    if (item_name) {
        QWidget* widget = item_name->widget();
        QLabel* label = dynamic_cast<QLabel*>(widget);
        FlashBackground(label);
    }

    QLayoutItem* item_raw = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::VALUE_BINARY) + NCOL * channel.Card());
    if (item_raw) {
        QWidget* widget = item_raw->widget();
        QLabel* label = dynamic_cast<QLabel*>(widget);
        label->setText(channel.Hex(value));
    }

    QLayoutItem* item_volt = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::VALUE_ADC) + NCOL * channel.Card());
    if (item_raw) {
        QWidget* widget = item_volt->widget();
        QLabel* label = dynamic_cast<QLabel*>(widget);
        label->setText(QString::number(channel.ADCValue(value), 'f', 2));
    }

    QLayoutItem* item_phy = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::VALUE_PHYSICAL) + NCOL * channel.Card());
    if (item_phy) {
        QWidget* widget = item_phy->widget();
        QLabel* label = dynamic_cast<QLabel*>(widget);
        label->setText(QString::number(channel.PhysicalValue(value), 'e', 2));
    }
}

void ADCGui::FlashBackground(QLabel *label) {
    label->setStyleSheet("QLabel {background: #3daee9}");
    QTimer::singleShot(40, this, [label]() { label->setStyleSheet("QLabel {background: solid black"); } );
}

QString ADCGui::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/Images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

void ADCGui::ShowPing() {
    ui_->frame_ping->setStyleSheet(FrameStyleSheet("frame_ping", "circle-blue-20x20"));
    QTimer::singleShot(500, this, [&]() { ui_->frame_ping->setStyleSheet("QFrame {}"); });
}

void ADCGui::ShowConnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "connected_90x90.png"));
}

void ADCGui::ShowDisconnected() {
    ui_->frame_connection->setStyleSheet(FrameStyleSheet("frame_connection", "disconnected_90x90.png"));
}

} // namespace
