#include "NSingleGui.h"
#include "ui_NSingleGui.h"

#include <QMessageBox>
#include <QStandardPaths>
#include <QDoubleSpinBox>

#include "Config.h"
#include "NSingleRepo.h"
#include "NSingleWidget.h"

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

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
    QMap<QString, hw_nsingle::NSingle*>::iterator it;
    for (it = nsingles_.begin(); it != nsingles_.end(); ++it) {
        delete it.value();
    }
}

void NSingleGui::ConnectGuiSignals() {
    qDebug() << "NSingleGui::ConnectGuiSignals";

    QObject::connect(ui_->pushButton_quit, &QPushButton::clicked, this, &NSingleGui::close);

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
    QObject::connect(ui_->doubleSpinBox_ch1_setpoint, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &NSingleGui::Channel1SetPointValueUpdated);
    QObject::connect(ui_->bitfield_ch1_setpoint, &BitFieldWidget::SIGNAL_Changed, this, &NSingleGui::Channel1SetPointBitSetUpdated);

    QObject::connect(ui_->doubleSpinBox_ch1_tolerance, qOverload<double>(&QDoubleSpinBox::valueChanged), this, &NSingleGui::Channel1ToleranceValueUpdated);
    QObject::connect(ui_->bitfield_ch1_tolerance, &BitFieldWidget::SIGNAL_Changed, this, &NSingleGui::Channel1ToleranceBitSetUpdated);

    QObject::connect(ui_->spinBox_address_setpoint, qOverload<int>(&QSpinBox::valueChanged), this, &NSingleGui::AddressSetPointValueUpdated);
    QObject::connect(ui_->bitfield_address_setpoint, &BitFieldWidget::SIGNAL_Changed, this, &NSingleGui::AddressSetPointBitSetUpdated);
}

void NSingleGui::ChangeState(hw_nsingle::StateCommandType cmd) {
    try {
        nsingles_[selected_nsingle_]->ChangeState(cmd);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadAddress() {
    try {
        nsingles_[selected_nsingle_]->ReadAddress();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadAddressSetPoint() {
    try {
        nsingles_[selected_nsingle_]->ReadAddressSetPoint();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadChannel1() {
    try {
        nsingles_[selected_nsingle_]->ReadChannel1();
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadChannel1SetPoint() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            nsingles_[selected_nsingle_]->ReadChannel1SetPoint();
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadError() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            nsingles_[selected_nsingle_]->ReadError();
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadConfig() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            nsingles_[selected_nsingle_]->ReadConfig();
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ReadState() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            nsingles_[selected_nsingle_]->ReadState();
        }
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

void NSingleGui::Channel1SetPointValueUpdated() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::Measurement m(nsingles_[selected_nsingle_]->config().GetChannel1SetPointConfig());
            m.SetData(ui_->doubleSpinBox_ch1_setpoint->value(), Ch1SetPointSign());
            const int value = m.RawValue();
            if (value != ui_->bitfield_ch1_setpoint->GetValue()) {
                ui_->bitfield_ch1_setpoint->SetValue(value);
            }
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::Channel1SetPointBitSetUpdated(int value) {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::Measurement m(nsingles_[selected_nsingle_]->config().GetChannel1SetPointConfig());
            m.SetData(value, Ch1SetPointSign());
            if (std::abs(ui_->doubleSpinBox_ch1_setpoint->value() - m.InterpretedValue()) > 0.001) {
                ui_->doubleSpinBox_ch1_setpoint->setValue(m.InterpretedValue());
            }
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::Channel1ToleranceValueUpdated() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::Measurement m(nsingles_[selected_nsingle_]->config().GetChannel1ValueConfig());
            m.SetData(ui_->doubleSpinBox_ch1_tolerance->value(), true);
            const int value = m.RawValue();
            if (value != ui_->bitfield_ch1_tolerance->GetValue()) {
                ui_->bitfield_ch1_tolerance->SetValue(value);
            }
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::Channel1ToleranceBitSetUpdated(int value) {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::Measurement m(nsingles_[selected_nsingle_]->config().GetChannel1ValueConfig());
            m.SetData(value, true); // sign is not taken into account by the nsingle
            if (std::abs(ui_->doubleSpinBox_ch1_tolerance->value() - m.InterpretedValue()) > 0.001) {
                ui_->doubleSpinBox_ch1_tolerance->setValue(m.InterpretedValue());
            }
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::AddressSetPointValueUpdated(int value) {
    try {
        std::bitset<8> bits(value);
        hw_nsingle::AddressRegister a(bits);
        DisplayAddressSetPoint(a);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::AddressSetPointBitSetUpdated(int value) {
    try {
        std::bitset<8> bits(value);
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
        if (nsingles_.contains(selected_nsingle_)) {
            const int data = ui_->bitfield_ch1_setpoint->GetValue();
            nsingles_[selected_nsingle_]->WriteChannel1SetPoint(data, Ch1SetPointSign());
        }
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteChannel1Tolerance() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            std::bitset<16> bitset(ui_->bitfield_ch1_tolerance->GetValue());
            nsingles_[selected_nsingle_]->WriteChannel1Tolerance(bitset);
        }
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteAddressSetPoint() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            const std::bitset<8> data(ui_->bitfield_address_setpoint->GetValue());
            nsingles_[selected_nsingle_]->WriteAddressSetPoint(data);
        }
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::WriteConfig() {
    try {
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::Config config(qvariant_cast<hw_nsingle::Config::Kind>(ui_->comboBox_kind->currentData()),
                                   qvariant_cast<hw_nsingle::Config::Polarity>(ui_->comboBox_polarity->currentData()),
                                   qvariant_cast<hw_nsingle::Config::Filtering>(ui_->comboBox_filtering->currentData()),
                                   qvariant_cast<hw_nsingle::Config::Averaging>(ui_->comboBox_averaging->currentData()),
                                   qvariant_cast<hw_nsingle::Config::StateMonitoring>(ui_->comboBox_statemonitoring->currentData()));
            nsingles_[selected_nsingle_]->Write(config);
        }
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::UpdateConfig(hw_nsingle::Config config) {
    ui_->comboBox_kind->setCurrentIndex(ui_->comboBox_kind->findData(config.kind()));
    ui_->comboBox_polarity->setCurrentIndex(ui_->comboBox_polarity->findData(config.polarity()));
    ui_->comboBox_filtering->setCurrentIndex(ui_->comboBox_filtering->findData(config.filtering()));
    ui_->comboBox_averaging->setCurrentIndex(ui_->comboBox_averaging->findData(config.averaging()));
    ui_->comboBox_statemonitoring->setCurrentIndex(ui_->comboBox_statemonitoring->findData(config.statemonitoring()));
}

void NSingleGui::UpdateError(hw_nsingle::Error error) {
    ui_->comboBox_errors->clear();
    ui_->comboBox_errors->addItems(error.active());
}

void NSingleGui::UpdateState(hw_nsingle::StateReply state) {
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
    ui_->bitfield_address_value->SetValue(address.bitset().to_ulong());
    ui_->lineEdit_address_value->setText(QString::number(address.bitset().to_ulong()));
}

void NSingleGui::DisplayAddressSetPoint(const hw_nsingle::AddressRegister& address) {
    if (static_cast<int>(address.bitset().to_ulong()) != ui_->bitfield_address_setpoint->GetValue()) {
        ui_->bitfield_address_setpoint->SetValue(address.bitset().to_ulong());
    }
    if (static_cast<int>(address.bitset().to_ulong()) != ui_->spinBox_address_setpoint->value()) {
        ui_->spinBox_address_setpoint->setValue(address.bitset().to_ulong());
    }
}

void NSingleGui::DisplayChannel1Value(const hw_nsingle::Measurement& m) {
    qDebug() << "DisplayChannel1Value " << m.RawValue();
    ui_->lineEdit_ch1_value->setText(QString::number(m.InterpretedValue(), 'f', 2));
    ui_->bitfield_ch1_value->SetValue(m.RawValue());
    ui_->label_ch1_value_sign->setText(SignToString(m.sign()));
}

void NSingleGui::DisplayChannel1SetPoint(const hw_nsingle::Measurement& m) {
    ui_->doubleSpinBox_ch1_setpoint->setValue(m.InterpretedValue());
    ui_->pushButton_ch1_setpoint_sign->setText(SignToString(m.sign()));
    ui_->bitfield_ch1_setpoint->SetValue(m.RawValue());
}

void NSingleGui::SetupDatabase() {
    qDebug() << "NSingleGui::SetupDatabase";

    if (!repo_) {
        repo_ = std::unique_ptr<medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo>(new medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo("_prod"));
    }
}

void NSingleGui::ReadNSingleState() {
    try {
        qDebug() << "NSingleGui::ReadNSingleState";
        if (nsingles_.contains(selected_nsingle_)) {
            hw_nsingle::NSingle *nsingle = nsingles_[selected_nsingle_];
            nsingle->ReadState();
            nsingle->ReadConfig();
            nsingle->ReadError();
            nsingle->ReadAddress();
            nsingle->ReadAddressSetPoint();
            nsingle->ReadChannel1();
            nsingle->ReadChannel1SetPoint();
        }
    }
    catch(std::exception&  exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::ConnectNSingleSignals(QString name) {
    qDebug() << "NSingleGui::ConnectNSingleSignals for " << name;
    if (nsingles_.contains(name)) {
        hw_nsingle::NSingle *nsingle = nsingles_[name];
        ui_->doubleSpinBox_ch1_setpoint->setSingleStep(nsingle->config().GetChannel1SetPointConfig().min() * nsingle->config().GetChannel1SetPointConfig().bit_value());
        ui_->doubleSpinBox_ch1_setpoint->setMaximum(nsingle->config().GetChannel1SetPointConfig().max() * nsingle->config().GetChannel1SetPointConfig().bit_value());
        ui_->doubleSpinBox_ch1_tolerance->setSingleStep(nsingle->config().GetChannel1ValueConfig().min() * nsingle->config().GetChannel1ValueConfig().bit_value());
        ui_->doubleSpinBox_ch1_tolerance->setMaximum(nsingle->config().GetChannel1ValueConfig().max() * nsingle->config().GetChannel1ValueConfig().bit_value());
        QObject::connect(nsingle, qOverload<const hw_nsingle::Measurement&>(&hw_nsingle::NSingle::SIGNAL_Channel1SetPoint), this, &NSingleGui::DisplayChannel1SetPoint);
        QObject::connect(nsingle, qOverload<const hw_nsingle::Measurement&>(&hw_nsingle::NSingle::SIGNAL_Channel1Value), this, &NSingleGui::DisplayChannel1Value);
        QObject::connect(nsingle, &hw_nsingle::NSingle::SIGNAL_AddressSetPoint, this, &NSingleGui::DisplayAddressSetPoint);
        QObject::connect(nsingle, &hw_nsingle::NSingle::SIGNAL_AddressValue, this, &NSingleGui::DisplayAddressValue);
        QObject::connect(nsingle, &hw_nsingle::NSingle::SIGNAL_ConfigUpdated, this, &NSingleGui::UpdateConfig);
        QObject::connect(nsingle, &hw_nsingle::NSingle::SIGNAL_ErrorUpdated, this, &NSingleGui::UpdateError);
        QObject::connect(nsingle, &hw_nsingle::NSingle::SIGNAL_StateUpdated, this, &NSingleGui::UpdateState);

        ui_->bitfield_address_setpoint->Setup(15, 8, false, BITFIELDALIGNMENT::RIGHT);
        ui_->bitfield_address_value->Setup(15, 8, true, BITFIELDALIGNMENT::RIGHT);

        hw_nsingle::MeasurementConfig ch1_setpoint_config = nsingle->config().GetChannel1SetPointConfig();
        ui_->bitfield_ch1_setpoint->Setup(15, ch1_setpoint_config.n_value_bits(), false, BITFIELDALIGNMENT::RIGHT);
        for (auto iBit = 0; iBit < ch1_setpoint_config.lsb_bit(); ++iBit) {
            ui_->bitfield_ch1_setpoint->Disable(iBit);
        }

        hw_nsingle::MeasurementConfig ch1_value_config = nsingle->config().GetChannel1ValueConfig();
        ui_->bitfield_ch1_value->Setup(15, ch1_value_config.n_value_bits(), true, BITFIELDALIGNMENT::RIGHT);
        for (auto iBit = 0; iBit < ch1_value_config.lsb_bit(); ++iBit) {
            ui_->bitfield_ch1_value->Disable(iBit);
        }

        hw_nsingle::MeasurementConfig ch1_tolerance_config = nsingle->config().GetChannel1ValueConfig();
        ui_->bitfield_ch1_tolerance->Setup(15, ch1_tolerance_config.n_value_bits(), false, BITFIELDALIGNMENT::RIGHT);
        for (auto iBit = 0; iBit < ch1_value_config.lsb_bit(); ++iBit) {
            ui_->bitfield_ch1_tolerance->Disable(iBit);
        }

    } else {
        qWarning() << "NSingleGui::ConnectNSingleSignals Key does not exist: " << name;
    }
}

void NSingleGui::DeconnectNSingleSignals(QString name) {
    qDebug() << "NSingleGui::DeconnectNSingleSignals for " << name;
    if (nsingles_.contains(name)) {
        hw_nsingle::NSingle *nsingle = nsingles_[name];
        QObject::disconnect(nsingle, qOverload<const hw_nsingle::Measurement&>(&hw_nsingle::NSingle::SIGNAL_Channel1SetPoint), this, &NSingleGui::DisplayChannel1SetPoint);
        QObject::disconnect(nsingle, qOverload<const hw_nsingle::Measurement&>(&hw_nsingle::NSingle::SIGNAL_Channel1Value), this, &NSingleGui::DisplayChannel1Value);
        QObject::disconnect(nsingle, &hw_nsingle::NSingle::SIGNAL_AddressSetPoint, this, &NSingleGui::DisplayAddressSetPoint);
        QObject::disconnect(nsingle, &hw_nsingle::NSingle::SIGNAL_AddressValue, this, &NSingleGui::DisplayAddressValue);
        QObject::disconnect(nsingle, &hw_nsingle::NSingle::SIGNAL_ConfigUpdated, this, &NSingleGui::UpdateConfig);
        QObject::disconnect(nsingle, &hw_nsingle::NSingle::SIGNAL_ErrorUpdated, this, &NSingleGui::UpdateError);
        QObject::disconnect(nsingle, &hw_nsingle::NSingle::SIGNAL_StateUpdated, this, &NSingleGui::UpdateState);
        } else {
        qWarning() << "NSingleGui::DisconnectNSingleSignals Key does not exist: " << name;
    }
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
        QStringList nsingles = repo_->GetHardNSingles();
        qDebug() << "NSingleGui::CreateNSingles Found these ones in db: " << nsingles;
        for (int iNSingle = 0; iNSingle < nsingles.size(); ++iNSingle) {
            QString name = nsingles.at(iNSingle);
            widgets_[name] = new NSingleWidget(name);
            gridLayout_nsingles_->addLayout(widgets_[name]->GetLayout(), iNSingle / NCOL, iNSingle % NCOL, Qt::AlignHCenter);

            hw_nsingle::NSingleConfig config = repo_->GetNSingleConfig(name);
            nsingles_[name] = new hw_nsingle::NSingle(config);

            assert(QObject::connect(nsingles_[name], SIGNAL(SIGNAL_Connected()), widgets_[name], SLOT(Connected())));
            assert(QObject::connect(nsingles_[name], SIGNAL(SIGNAL_Disconnected()), widgets_[name], SLOT(Disconnected())));
            assert(QObject::connect(nsingles_[name], SIGNAL(SIGNAL_ReplyReceived()), widgets_[name], SLOT(Ping())));
            assert(QObject::connect(nsingles_[name], SIGNAL(SIGNAL_On()) , widgets_[name], SLOT(On())));
            assert(QObject::connect(nsingles_[name], SIGNAL(SIGNAL_Off()) , widgets_[name], SLOT(Off())));
            assert(QObject::connect(widgets_[name], SIGNAL(Selected(QString)), this, SLOT(SelectNSingle(QString))));
        }
        if (!nsingles.empty()) {
            widgets_[nsingles.at(0)]->Select();
        }
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void NSingleGui::SelectNSingle(QString name) {
     if (!nsingles_.contains(name)) {
         qWarning() << "NSingleGui::SelectNSingle Did not found nsingle " << name;
         return;
     }
    qDebug() << "NSingleGui::SelectNSingle Selecting " << name;

    DeconnectNSingleSignals(selected_nsingle_);
    selected_nsingle_ = name;
    QMapIterator<QString, NSingleWidget*> it(widgets_);
    while (it.hasNext()) {
        it.next();
        if (it.value()->name() != name) it.value()->Deselect();
    }
    ui_->spinBox_address_setpoint->setValue(0);
    ui_->doubleSpinBox_ch1_setpoint->setValue(0.0);
    ui_->doubleSpinBox_ch1_tolerance->setValue(0.0);
    ui_->lineEdit_ch1_value->setText("0.0");
    ui_->lineEdit_address_value->setText("0");
    ConnectNSingleSignals(name);
    ReadNSingleState();
    ReadConfig();
    ReadChannel1();
    ReadChannel1SetPoint();
    ReadError();
}

} // ns
