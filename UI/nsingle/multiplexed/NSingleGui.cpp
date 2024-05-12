#include "NSingleGui.h"
#include "ui_NSingleGui.h"

#include <QMessageBox>
#include <QStandardPaths>

#include "Config.h"
#include "NSingleRepo.h"
#include "NSingleWidget.h"
#include "MultiplexedNSingle.h"

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::multiplexed {

NSingleGui::NSingleGui(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::NSingleGui)
{
    ui_->setupUi(this);
    SetupLayout();
    SetupDatabase();
    CreateNSingles();
    FillComboBoxes();
    ConnectGuiSignals();
}

NSingleGui::~NSingleGui()
{
    delete nsingle_;
}

void NSingleGui::ConnectGuiSignals() {
    qDebug() << "NSingleGui::ConnectGuiSignals";

    // State commands
    QObject::connect(ui_->pushButton_on, &QPushButton::clicked, this, [&]() { ChangeState(hw_nsingle::StateCommandType::On); });
    QObject::connect(ui_->pushButton_off, &QPushButton::clicked, this, [&]() { ChangeState(hw_nsingle::StateCommandType::Off); });
    QObject::connect(ui_->pushButton_standBy, &QPushButton::clicked, this, [&]() { ChangeState(hw_nsingle::StateCommandType::StdBy); });
    QObject::connect(ui_->pushButton_reset, &QPushButton::clicked, this, [&]() { ChangeState(hw_nsingle::StateCommandType::Reset); });

    // Read commands
    QObject::connect(ui_->pushButton_read_address_value, &QPushButton::clicked, this, &NSingleGui::ReadAddress);
    QObject::connect(ui_->pushButton_read_address_setpoint, &QPushButton::clicked, this, &NSingleGui::ReadAddressSetPoint);
    QObject::connect(ui_->pushButton_read_ch1_value, &QPushButton::clicked, this, &NSingleGui::ReadChannel1);
    QObject::connect(ui_->pushButton_read_ch1_setpoint, &QPushButton::clicked, this, &NSingleGui::ReadChannel1SetPoint);
    QObject::connect(ui_->pushButton_read_error, &QPushButton::clicked, this, &NSingleGui::ReadError);
    QObject::connect(ui_->pushButton_read_config, &QPushButton::clicked, this, &NSingleGui::ReadConfig);
    QObject::connect(ui_->pushButton_read_state, &QPushButton::clicked, this, &NSingleGui::ReadState);

    // Write commands
    QObject::connect(ui_->pushButton_write_config, &QPushButton::clicked, this, &NSingleGui::WriteConfig);
    QObject::connect(ui_->pushButton_write_address_setpoint, &QPushButton::clicked, this, &NSingleGui::WriteAddressSetPoint);
    QObject::connect(ui_->pushButton_write_ch1_setpoint, &QPushButton::clicked, this, &NSingleGui::WriteChannel1SetPoint);
    QObject::connect(ui_->pushButton_write_ch1_tolerance, &QPushButton::clicked, this, &NSingleGui::WriteChannel1Tolerance);
    QObject::connect(ui_->pushButton_ch1_setpoint_sign, &QPushButton::clicked, this, [&]() {
        if (ui_->pushButton_ch1_setpoint_sign->text() == QString("+")) {
            ui_->pushButton_ch1_setpoint_sign->setText("-");
        } else {
            ui_->pushButton_ch1_setpoint_sign->setText("+");
        }
    });

    // SYNC VALUES IN GUI
    QObject::connect(ui_->lineEdit_ch1_setpoint, &QLineEdit::returnPressed, this, &NSingleGui::Channel1SetPointValueUpdated);
    QObject::connect(ui_->lineEdit_ch1_setpoint_bits, &QLineEdit::returnPressed, this, &NSingleGui::Channel1SetPointBitSetUpdated);

    QObject::connect(ui_->lineEdit_ch1_tolerance, &QLineEdit::returnPressed, this, &NSingleGui::Channel1ToleranceValueUpdated);
    QObject::connect(ui_->lineEdit_ch1_tolerance_bits, &QLineEdit::returnPressed, this, &NSingleGui::Channel1ToleranceBitSetUpdated);

    QObject::connect(ui_->lineEdit_address_setPoint, &QLineEdit::returnPressed, this, &NSingleGui::AddressSetPointValueUpdated);
    QObject::connect(ui_->lineEdit_address_setPoint_bits, &QLineEdit::returnPressed, this, &NSingleGui::AddressSetPointBitSetUpdated);
}

void NSingleGui::ChangeState(hw_nsingle::StateCommandType cmd) {
    try {
        qDebug() << "GUi Changestate " << selected_nsingle_;
        nsingle_->ChangeState(selected_nsingle_, cmd);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadAddress() {
    try {
        nsingle_->ReadAddress(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadAddressSetPoint() {
    try {
        nsingle_->ReadAddressSetPoint(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadChannel1() {
    try {
        nsingle_->ReadChannel1(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadChannel1SetPoint() {
    try {
        nsingle_->ReadChannel1SetPoint(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadError() {
    try {
        nsingle_->ReadError(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadConfig() {
    try {
        nsingle_->ReadConfig(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadState() {
    try {
        nsingle_->ReadState(selected_nsingle_);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}


QString NSingleGui::SignToString(bool sign) const {
    if (sign) {
        return QString("+");
    } else {
        return QString("-");
    }
}

bool NSingleGui::Ch1SetPointSign() const {
    return (ui_->pushButton_ch1_setpoint_sign->text() == QString("+"));
}

bool NSingleGui::Ch1ValueSign() const {
    return (ui_->label_ch1_value_sign->text() == QString("+"));
}

bool NSingleGui::Ch2ValueSign() const {
    return (ui_->label_ch2_value_sign->text() == QString("+"));
}

void NSingleGui::Channel1SetPointValueUpdated() {
    try {
        hw_nsingle::Measurement m(nsingle_->GetNSingleConfig().GetChannel1SetPointConfig());
        m.SetData(ui_->lineEdit_ch1_setpoint->text().toDouble(), Ch1SetPointSign());
        DisplayChannel1SetPoint("", m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::Channel1SetPointBitSetUpdated() {
    try {
        hw_nsingle::Measurement m(nsingle_->GetNSingleConfig().GetChannel1SetPointConfig());
        std::bitset<15> bits(ui_->lineEdit_ch1_setpoint_bits->text().toStdString());
        m.SetData(static_cast<int>(bits.to_ulong()), Ch1SetPointSign());
        DisplayChannel1SetPoint("", m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::Channel1ToleranceValueUpdated() {
}

void NSingleGui::Channel1ToleranceBitSetUpdated() {
}

void NSingleGui::AddressSetPointValueUpdated() {
    try {
        std::bitset<8> bits(ui_->lineEdit_address_setPoint->text().toInt());
        hw_nsingle::AddressRegister a(bits);
        DisplayAddressSetPoint(a);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::AddressSetPointBitSetUpdated() {
    try {
        std::bitset<8> bits(ui_->lineEdit_address_setPoint_bits->text().toStdString());
        hw_nsingle::AddressRegister a(bits);
        DisplayAddressSetPoint(a);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::FillComboBoxes() {
    qDebug() << "NSingleGui::FillComboBoxes";
    QMap<QString, hw_nsingle::Config::Kind> kinds = hw_nsingle::Config::kinds();
    QMap<QString, hw_nsingle::Config::Kind>::iterator it_kinds;
    for (it_kinds = kinds.begin(); it_kinds != kinds.end(); ++it_kinds) {
        ui_->comboBox_kind->addItem(it_kinds.key(), it_kinds.value());
    }

    QMap<QString, hw_nsingle::Config::Polarity> polarities = hw_nsingle::Config::polarities();
    QMap<QString, hw_nsingle::Config::Polarity>::iterator it_pol;
    for (it_pol = polarities.begin(); it_pol != polarities.end(); ++it_pol) {
        ui_->comboBox_polarity->addItem(it_pol.key(), it_pol.value());
    }

    QMap<QString, hw_nsingle::Config::Filtering> filterings = hw_nsingle::Config::filterings();
    QMap<QString, hw_nsingle::Config::Filtering>::iterator it_fil;
    for (it_fil = filterings.begin(); it_fil != filterings.end(); ++it_fil) {
        ui_->comboBox_filtering->addItem(it_fil.key(), it_fil.value());
    }

    QMap<QString, hw_nsingle::Config::Averaging> averagings = hw_nsingle::Config::averagings();
    QMap<QString, hw_nsingle::Config::Averaging>::iterator it_ave;
    for (it_ave = averagings.begin(); it_ave != averagings.end(); ++it_ave) {
        ui_->comboBox_averaging->addItem(it_ave.key(), it_ave.value());
    }

    QMap<QString, hw_nsingle::Config::StateMonitoring> statemonitorings = hw_nsingle::Config::statemonitorings();
    QMap<QString, hw_nsingle::Config::StateMonitoring>::iterator it_thr;
    for (it_thr = statemonitorings.begin(); it_thr != statemonitorings.end(); ++it_thr) {
        ui_->comboBox_statemonitoring->addItem(it_thr.key(), it_thr.value());
    }
}

void NSingleGui::WriteChannel1SetPoint() {
    try {
        const int data = std::bitset<15>(ui_->lineEdit_ch1_setpoint_bits->text().toStdString()).to_ulong();
        nsingle_->WriteChannel1SetPoint(selected_nsingle_, data, Ch1SetPointSign());
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteChannel1Tolerance() {
    try {
        std::bitset<16> data = std::bitset<16>(ui_->lineEdit_ch1_tolerance_bits->text().toStdString()).to_ulong();
        nsingle_->WriteChannel1Tolerance(selected_nsingle_, data);
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteAddressSetPoint() {
    try {
            const std::bitset<8> data(ui_->lineEdit_address_setPoint_bits->text().toStdString());
            nsingle_->WriteAddressSetPoint(selected_nsingle_, data);
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteConfig() {
    try {
        hw_nsingle::Config config(qvariant_cast<hw_nsingle::Config::Kind>(ui_->comboBox_kind->currentData()),
                      qvariant_cast<hw_nsingle::Config::Polarity>(ui_->comboBox_polarity->currentData()),
                      qvariant_cast<hw_nsingle::Config::Filtering>(ui_->comboBox_filtering->currentData()),
                      qvariant_cast<hw_nsingle::Config::Averaging>(ui_->comboBox_averaging->currentData()),
                      qvariant_cast<hw_nsingle::Config::StateMonitoring>(ui_->comboBox_statemonitoring->currentData()));
        nsingle_->Write(selected_nsingle_, config);
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::UpdateConfig(QString name, hw_nsingle::Config config) {
    (void) name;
    ui_->comboBox_kind->setCurrentIndex(ui_->comboBox_kind->findData(config.kind()));
    ui_->comboBox_polarity->setCurrentIndex(ui_->comboBox_polarity->findData(config.polarity()));
    ui_->comboBox_filtering->setCurrentIndex(ui_->comboBox_filtering->findData(config.filtering()));
    ui_->comboBox_averaging->setCurrentIndex(ui_->comboBox_averaging->findData(config.averaging()));
    ui_->comboBox_statemonitoring->setCurrentIndex(ui_->comboBox_statemonitoring->findData(config.statemonitoring()));
}

void NSingleGui::UpdateError(QString name, hw_nsingle::Error error) {
    (void) name;
    ui_->comboBox_errors->clear();
    ui_->comboBox_errors->addItems(error.active());
}

void NSingleGui::UpdateState(QString name, hw_nsingle::StateReply state) {
    (void) name;
    ui_->standByRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::StdBy));
    ui_->offRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Off));
    ui_->onRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::On));
    ui_->localRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Local));
    ui_->distanceRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Remote));
    ui_->okRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Ok));
    ui_->errorRadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Error));
    ui_->switch1RadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Switch1));
    ui_->switch2RadioButton->setChecked(state.Channel(hw_nsingle::StateReplyChannel::Switch2));
}

void NSingleGui::DisplayAddressValue(const hw_nsingle::AddressRegister& address) {
    ui_->lineEdit_address_value_bits->setText(QString::fromStdString(address.bitset().to_string()));
    ui_->lineEdit_address_value->setText(QString::number(address.bitset().to_ulong()));
}

void NSingleGui::DisplayAddressSetPoint(const hw_nsingle::AddressRegister& address) {
    ui_->lineEdit_address_setPoint_bits->setText(QString::fromStdString(address.bitset().to_string()));
    ui_->lineEdit_address_setPoint->setText(QString::number(address.bitset().to_ulong()));
}

void NSingleGui::DisplayChannel1Value(QString name, const hw_nsingle::Measurement& m) {
    (void) name;
    const std::bitset<15> bits(m.RawValue());
    ui_->lineEdit_ch1_value_bits->setText(QString::fromStdString(bits.to_string()));
    ui_->lineEdit_ch1_value->setText(QString::number(m.InterpretedValue(), 'f', 2));
    ui_->label_ch1_value_sign->setText(SignToString(m.sign()));
}

void NSingleGui::DisplayChannel1SetPoint(QString name, const hw_nsingle::Measurement& m) {
    (void) name;
    const std::bitset<15> bits(m.RawValue());
    ui_->lineEdit_ch1_setpoint_bits->setText(QString::fromStdString(bits.to_string()));
    ui_->lineEdit_ch1_setpoint->setText(QString::number(m.InterpretedValue(), 'f', 1));
    ui_->pushButton_ch1_setpoint_sign->setText(SignToString(m.sign()));
}

void NSingleGui::SetupDatabase() {
    qDebug() << "NSingleGui::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo>(new medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo("proddatabase"));
    }
}

void NSingleGui::ReadNSingleState() {
    try {
        qDebug() << "NSingleGui::ReadNSingleState";
        nsingle_->ReadState(selected_nsingle_);
        nsingle_->ReadConfig(selected_nsingle_);
        nsingle_->ReadError(selected_nsingle_);
        nsingle_->ReadAddress(selected_nsingle_);
        nsingle_->ReadAddressSetPoint(selected_nsingle_);
        nsingle_->ReadChannel1(selected_nsingle_);
        nsingle_->ReadChannel1SetPoint(selected_nsingle_);
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ConnectNSingleSignals() {
    QObject::connect(nsingle_, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1SetPoint), this, &NSingleGui::DisplayChannel1SetPoint);
    QObject::connect(nsingle_, qOverload<QString, const hw_nsingle::Measurement&>(&hw_nsingle::MultiplexedNSingle::SIGNAL_Channel1Value), this, &NSingleGui::DisplayChannel1Value);
    QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_AddressSetPoint, this, &NSingleGui::DisplayAddressSetPoint);
    QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_AddressValue, this, &NSingleGui::DisplayAddressValue);
    QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_ConfigUpdated, this, &NSingleGui::UpdateConfig);
    QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_ErrorUpdated, this, &NSingleGui::UpdateError);
    QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_StateUpdated, this, &NSingleGui::UpdateState);
}

void NSingleGui::SetupLayout() {
    QWidget *widget = new QWidget;
    gridLayout_nsingles_ = new QGridLayout(widget);
    widget->setLayout(gridLayout_nsingles_);
    ui_->scrollArea->setWidget(widget);
}

void NSingleGui::CreateNSingles() {
    try {
        const int NCOL = 7;
        QString nsingle_name("Multiplexed_1");
        qDebug() << "NSingleGui::CreateNSingles Found these ones in db: " << nsingle_name;

        hw_nsingle::MultiplexConfig multiplex_config = repo_->GetMultiplexConfig(nsingle_name);
        hw_nsingle::NSingleConfig nsingle_config = repo_->GetNSingleConfig(nsingle_name);
        std::vector<QString> channel_names = multiplex_config.channels();
        nsingle_ = new hw_nsingle::MultiplexedNSingle(nsingle_config, multiplex_config);
        ConnectNSingleSignals();

        int iNSingle = 0;
        for (QString& ch :  channel_names) {
            widgets_[ch] = new NSingleWidget(ch);
            gridLayout_nsingles_->addLayout(widgets_[ch]->GetLayout(), iNSingle / NCOL, iNSingle % NCOL, Qt::AlignHCenter);

            QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_Connected, widgets_[ch], &NSingleWidget::Connected);
            QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_Disconnected, widgets_[ch], &NSingleWidget::Disconnected);
            QObject::connect(nsingle_, &hw_nsingle::MultiplexedNSingle::SIGNAL_ReplyReceived, widgets_[ch], &NSingleWidget::Ping);
            QObject::connect(widgets_[ch], &NSingleWidget::Selected, this, &NSingleGui::SelectNSingle);
            iNSingle++;
        }
        widgets_[channel_names.at(0)]->Select();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::SelectNSingle(QString name) {
    selected_nsingle_ = name;
    QMapIterator<QString, NSingleWidget*> it(widgets_);
    while (it.hasNext()) {
        it.next();
        if (it.value()->name() != name) it.value()->Deselect();
    }
    ReadNSingleState();
    ReadConfig();
    ReadChannel1();
    ReadChannel1SetPoint();
    ReadError();
}

} // ns
