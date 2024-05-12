#include "ADCSim.h"
#include "ui_ADCSim.h"

#include <QStandardPaths>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::simulators::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;


ADCSim::ADCSim(VirtualADC *adc, QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::ADCSim),
    settings_(std::unique_ptr<QSettings>(new QSettings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat))),
    adc_(adc)
{
    ui_->setupUi(this);
    setWindowTitle("ADCSim");
    SetupLayout();
    SetupDatabase();
    auto channels = GetADCChannels();
    adc_->SetChannels(channels);
    DisplayUIChannels(channels);

    QObject::connect(ui_->pushButton_connect, &QPushButton::clicked, this, [&](bool checked) { adc_->SetConnected(checked); });
}

ADCSim::~ADCSim()
{
}

void ADCSim::ReadRequest(QString name) {
    if (!adc_->ChannelExist(name)) {
        qWarning() << "ADCSim::ReadRequest Channel does not exist " << name;
        return;
    }

    hw_adc::Channel channel = adc_->GetChannel(name);

    QLayoutItem* item_name = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::NAME) + NCOL * channel.Card());
    if (item_name) {
        QWidget* widget = item_name->widget();
        QLabel* label = dynamic_cast<QLabel*>(widget);
        FlashBackground(label);
    }

    QLayoutItem* item_phy = gridLayout_channels->itemAtPosition(1 + channel.Address(), static_cast<int>(ADC_COL::VALUE_PHYSICAL) + NCOL * channel.Card());
    if (item_phy) {
        QWidget* widget = item_phy->widget();
        QDoubleSpinBox* sb = dynamic_cast<QDoubleSpinBox*>(widget);
        emit SIGNAL_ReplyToReadRequest(name, sb->value(), "unit");
    } else {
        qWarning() << "ADCSim::ReadRequest Did not find physical value";
    }
}


void ADCSim::SetupLayout() {
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

void ADCSim::SetupDatabase() {
    qDebug() << "ADCSim::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<hw_adc::ADCRepo>(new hw_adc::ADCRepo("_prod"));
    }
    if (!repo_->Connect(5)) {
        QMessageBox::critical(this, "ADC", "Connection to database failed");
    }
}

std::vector<hw_adc::Channel>  ADCSim::GetADCChannels() {
    qDebug() << "ADCSim::GetADCChannels";
    std::vector<hw_adc::Channel> channels;
    try {
        QStringList names = repo_->GetADCChannelNames();
        for (QString& name : names) {
            channels.push_back(repo_->GetADCChannel(name));
            qDebug() << "ADCSim::GetADCChannels Found channel: " << name;
        }
    }
    catch (std::exception& exc) {
        //ui_->messagePlainTextEdit->appendPlainText(exc.what());
    }
    return channels;
}

void ADCSim::DisplayUIChannels(const std::vector<hw_adc::Channel>& channels) {
    qDebug() << "ADCSim::DisplayUIChannels";
    QFont header_font("LKLUG", 10, QFont::Bold);

    for (int icol = 0; icol < 4; ++icol) {
        gridLayout_channels->addWidget(new QLabel("Ch"), 0, icol * NCOL + static_cast<int>(ADC_COL::CHANNEL));
        gridLayout_channels->addWidget(new QLabel("Name"), 0, icol * NCOL + static_cast<int>(ADC_COL::NAME));
        gridLayout_channels->addWidget(new QLabel("Physical"), 0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_PHYSICAL));
        gridLayout_channels->addWidget(new QLabel("Unit"), 0, icol * NCOL + static_cast<int>(ADC_COL::UNIT_PHYSICAL));
        QLabel *lSpacer = new QLabel("");
        lSpacer->setFixedWidth(40);
        gridLayout_channels->addWidget(lSpacer, 0, icol * NCOL + static_cast<int>(ADC_COL::SPACER));

        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::CHANNEL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::NAME))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::VALUE_PHYSICAL))->widget())->setFont(header_font);
        dynamic_cast<QLabel*>(gridLayout_channels->itemAtPosition(0, icol * NCOL + static_cast<int>(ADC_COL::UNIT_PHYSICAL))->widget())->setFont(header_font);
    }

    for (const hw_adc::Channel& ch : channels) {
        gridLayout_channels->addWidget(new QLabel(QString::number(ch.Address())), 1 + ch.Address(), static_cast<int>(ADC_COL::CHANNEL) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Name()), 1 + ch.Address(), static_cast<int>(ADC_COL::NAME) + NCOL * ch.Card());
        QDoubleSpinBox *sp = new QDoubleSpinBox;
        sp->setValue(ch.SimValue());
        sp->setSingleStep(ch.SimStep());
        sp->setRange(-100*ch.SimStep(), 100*ch.SimStep());
        gridLayout_channels->addWidget(sp, 1 + ch.Address(), static_cast<int>(ADC_COL::VALUE_PHYSICAL) + NCOL * ch.Card());
        gridLayout_channels->addWidget(new QLabel(ch.Unit()), 1 + ch.Address(), static_cast<int>(ADC_COL::UNIT_PHYSICAL) + NCOL * ch.Card());

     }
}

void ADCSim::FlashBackground(QLabel *label) {
    label->setStyleSheet("QLabel {background: #3daee9}");
    QTimer::singleShot(40, this, [label]() { label->setStyleSheet("QLabel {background: solid black"); } );
}

QString ADCSim::FrameStyleSheet(QString name, QString image) const {
    QString str ("QFrame#" + name + " {"
                 "background-image: url(:/images/" + image + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    return str;
}

} // namespace
