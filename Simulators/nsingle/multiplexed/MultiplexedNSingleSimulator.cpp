#include "MultiplexedNSingleSimulator.h"
#include "ui_MultiplexedNSingleSimulator.h"

#include <QMessageBox>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::simulators::nsingle::multiplexed {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

MultiplexedNSingleSimulator::MultiplexedNSingleSimulator(hw_nsingle::VirtualNSingle *nsingle, QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::MultiplexedNSingleSimulator),
    nsingle_(nsingle)
{
    ui_->setupUi(this);
    ui_->label_name->setText(nsingle->name());
    ConnectGuiSignals();
    ping_timer_.setInterval(2000);
    ping_timer_.setSingleShot(false);
    if (ui_->pushButton_ping->isChecked()) ping_timer_.start();
}

MultiplexedNSingleSimulator::~MultiplexedNSingleSimulator()
{

}

void MultiplexedNSingleSimulator::ConnectGuiSignals() {
    qDebug() << "MultiplexedNSingleSimulator::ConnectGuiSignals";

    QObject::connect(ui_->pushButton_conn, &QPushButton::clicked, this, [&]() {
        if (connected_) {
            connected_ = false;
            emit nsingle_->SIGNAL_Disconnected();
        } else {
            connected_ = true;
            emit nsingle_->SIGNAL_Connected();
        }});
    QObject::connect(ui_->pushButton_ping, &QPushButton::clicked, this, [&](bool checked) { if (checked) ping_timer_.start(); else ping_timer_.stop(); });

    // User changes to channel1 values and address
    QObject::connect(ui_->pushButton_ch1_value_sign, &QPushButton::clicked, this, [&]() {
        if (Ch1ValueSign()) { ui_->pushButton_ch1_value_sign->setText("-"); }
        else { ui_->pushButton_ch1_value_sign->setText("+"); }});
    QObject::connect(ui_->pushButton_ch1_value_inc, &QPushButton::clicked, this, &MultiplexedNSingleSimulator::IncrementChannel1Value);
    QObject::connect(ui_->pushButton_ch1_value_dec, &QPushButton::clicked, this, &MultiplexedNSingleSimulator::DecrementChannel1Value);
    QObject::connect(ui_->lineEdit_ch1_value, &QLineEdit::returnPressed, this, &MultiplexedNSingleSimulator::Channel1ValueUpdated);
    QObject::connect(ui_->lineEdit_ch1_value_bits, &QLineEdit::returnPressed, this, &MultiplexedNSingleSimulator::Channel1ValueBitSetUpdated);
    QObject::connect(ui_->lineEdit_address_value, &QLineEdit::returnPressed, this, &MultiplexedNSingleSimulator::AddressValueUpdated);
    QObject::connect(ui_->lineEdit_address_value_bits, &QLineEdit::returnPressed, this, &MultiplexedNSingleSimulator::AddressValueBitSetUpdated);
    QObject::connect(&ping_timer_, &QTimer::timeout, this, [&]() {
        emit nsingle_->SIGNAL_CommandReadState("");
        emit nsingle_->SIGNAL_CommandReadChannel1(""); });

    // Commands from controller
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CheckConnection, this, [&]() {
        if (connected_) emit nsingle_->SIGNAL_Connected();
        else emit nsingle_->SIGNAL_Disconnected(); });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOn, this, [&]() { if (ui_->pushButton_on->isChecked()) {
            if (nsingle_->config().reset_at_on()) emit nsingle_->SIGNAL_CommandReset(nsingle_->name());
            ui_->onRadioButton->setChecked(true);
            ui_->offRadioButton->setChecked(false);
            QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_On(); }); }});
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandOff, this, [&]() { if (ui_->pushButton_off->isChecked()) {
            ui_->onRadioButton->setChecked(false);
            ui_->offRadioButton->setChecked(true);
            QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_Off(); }); }});
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReset, this, [&]() { if (ui_->pushButton_reset->isChecked()) {
            ui_->errorRadioButton->setChecked(false);
            ui_->okRadioButton->setChecked(true);
            QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_Ok(); }); }});
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadState, this, [&]() {
        if (ui_->pushButton_readstatus_active->isChecked()) {
        if (ui_->onRadioButton->isChecked()) QTimer::singleShot(100, this, [&]() { emit nsingle_->SIGNAL_On(); });
        if (ui_->offRadioButton->isChecked()) QTimer::singleShot(100, this, [&]() { emit nsingle_->SIGNAL_Off(); });
        if (ui_->errorRadioButton->isChecked()) QTimer::singleShot(100, this, [&]() { emit nsingle_->SIGNAL_Error(); });
        }
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandPing, this, [&]() {
        if (ui_->pushButton_readvalue_active->isChecked()) {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
            std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
            m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
            int offset = ui_->spinBox_signal_offset->value() + rand_.bounded(-1.0*ui_->spinBox_signal_variation->value(), 1 + ui_->spinBox_signal_variation->value());
            if (offset > 0) m.Increment(offset);
            if (offset < 0) m.Decrement(std::abs(offset));
            QTimer::singleShot(80, this, [&, m]() { emit nsingle_->SIGNAL_Channel1Value(m); });
       }
        if (ui_->pushButton_readstatus_active->isChecked()) {
            if (ui_->onRadioButton->isChecked()) QTimer::singleShot(80, this, [&]() { emit nsingle_->SIGNAL_On(); });
            if (ui_->offRadioButton->isChecked()) QTimer::singleShot(80, this, [&]() { emit nsingle_->SIGNAL_Off(); });
            if (ui_->errorRadioButton->isChecked()) QTimer::singleShot(80, this, [&]() { emit nsingle_->SIGNAL_Error(); });
        }
    });
    QObject::connect(nsingle_, qOverload<QString, int>(&hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1), this, [&](QString s, int n) {
        (void)s;
        if (ui_->pushButton_readvalue_active->isChecked()) {
        hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
        std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
        m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
        for (int i = 0; i < n; ++i) {
            int offset = ui_->spinBox_signal_offset->value() + rand_.bounded(-1.0*ui_->spinBox_signal_variation->value(), 1 + ui_->spinBox_signal_variation->value());
            if (offset > 0) m.Increment(offset);
            if (offset < 0) m.Decrement(std::abs(offset));
            QTimer::singleShot(80, this, [&, m]() { emit nsingle_->SIGNAL_Channel1Value(m); });
        }
       }
    });
    QObject::connect(nsingle_, qOverload<QString>(&hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1), this, [&]() {
        if (ui_->pushButton_readvalue_active->isChecked()) {
        hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
        std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
        m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
        int offset = ui_->spinBox_signal_offset->value() + rand_.bounded(-1.0*ui_->spinBox_signal_variation->value(), 1 + ui_->spinBox_signal_variation->value());
        if (offset > 0) m.Increment(offset);
        if (offset < 0) m.Decrement(std::abs(offset));
        QTimer::singleShot(80, this, [&, m]() { emit nsingle_->SIGNAL_Channel1Value(m); });
        }
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadChannel1SetPoint, this, [&]() {
        if (ui_->pushButton_read_setpoint_active->isChecked()) {
        hw_nsingle::Measurement m(nsingle_->config().GetChannel1SetPointConfig());
        std::bitset<15> bits(ui_->lineEdit_ch1_setpoint_bits->text().toStdString());
        m.SetData(static_cast<int>(bits.to_ulong()), Ch1SetPointSign());
        QTimer::singleShot(300, this, [&, m]() { emit nsingle_->SIGNAL_Channel1SetPoint(m); });
        }
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteChannel1SetPoint, this, [&](QString name, const hw_nsingle::Measurement& m) {
        if (ui_->pushButton_write_setpoint_active->isChecked()) {
            (void)name;
            DisplayChannel1SetPoint(m);
            QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_ExecutionFinished(); });
            if (ui_->pushButton_auto_transfer_setpoint->isChecked()) {
                hw_nsingle::Measurement m_value(nsingle_->config().GetChannel1ValueConfig());
                m_value.SetData(m.InterpretedValue(), m.sign());
                DisplayChannel1Value(m_value);
            }
        }
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteAndVerifyChannel1SetPoint, this, [&](QString name, const hw_nsingle::Measurement& m) {
        if (ui_->pushButton_write_setpoint_active->isChecked()) {
            (void)name;
            DisplayChannel1SetPoint(m);
            QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_ExecutionFinished(); });
            if (ui_->pushButton_auto_transfer_setpoint->isChecked()) {
                hw_nsingle::Measurement m_value(nsingle_->config().GetChannel1ValueConfig());
                m_value.SetData(m.InterpretedValue(), m.sign());
                DisplayChannel1Value(m_value);
            }
        }
        if (ui_->pushButton_read_setpoint_active->isChecked()) {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1SetPointConfig());
            std::bitset<15> bits(ui_->lineEdit_ch1_setpoint_bits->text().toStdString());
            m.SetData(static_cast<int>(bits.to_ulong()), Ch1SetPointSign());
            QTimer::singleShot(300, this, [&, m]() { emit nsingle_->SIGNAL_Channel1SetPoint(m); });
        }
    });

    // ADDRESSES
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandReadAddressSetPoint, this, [&]() {
        std::bitset<8> bits(ui_->lineEdit_address_setPoint->text().toStdString());
        hw_nsingle::AddressRegister a(bits);
        QTimer::singleShot(200, this, [&, a]() { emit nsingle_->SIGNAL_AddressSetPoint(a); });
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteAddressSetPoint, this, [&](QString name, std::bitset<8> address) {
        (void)name;
        hw_nsingle::AddressRegister a(address);
        DisplayAddressSetPoint(a);
        QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_ExecutionFinished(); });
        if (ui_->pushButton_auto_transfer_address_setpoint->isChecked()) {
            DisplayAddressValue(a);
        }
    });
    QObject::connect(nsingle_, &hw_nsingle::VirtualNSingle::SIGNAL_CommandWriteAndVerifyAddressSetPoint, this, [&](QString name, std::bitset<8> address) {
        (void)name;
        hw_nsingle::AddressRegister a(address);
        DisplayAddressSetPoint(a);
        QTimer::singleShot(200, this, [&]() { emit nsingle_->SIGNAL_ExecutionFinished(); });
        if (ui_->pushButton_auto_transfer_address_setpoint->isChecked()) {
            DisplayAddressValue(a);
        }
        QTimer::singleShot(200, this, [&, a]() { emit nsingle_->SIGNAL_AddressSetPoint(a); });
    });
}

QString MultiplexedNSingleSimulator::SignToString(bool sign) const {
    if (sign) {
        return QString("+");
    } else {
        return QString("-");
    }
}

bool MultiplexedNSingleSimulator::Ch1SetPointSign() const {
    return (ui_->label_ch1_setpoint_sign->text() == QString("+"));
}

bool MultiplexedNSingleSimulator::Ch1ValueSign() const {
    return (ui_->pushButton_ch1_value_sign->text() == QString("+"));
}

bool MultiplexedNSingleSimulator::Ch2ValueSign() const {
    return (ui_->label_ch2_value_sign->text() == QString("+"));
}

void MultiplexedNSingleSimulator::Channel1ValueUpdated() {
    try {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
            m.SetData(ui_->lineEdit_ch1_value->text().toDouble(), Ch1ValueSign());
            DisplayChannel1Value(m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::Channel1ValueBitSetUpdated() {
    try {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
            std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
            m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
            DisplayChannel1Value(m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::IncrementChannel1Value() {
    try {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
            std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
            m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
            m.Increment(1);
            DisplayChannel1Value(m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::DecrementChannel1Value() {
    try {
            hw_nsingle::Measurement m(nsingle_->config().GetChannel1ValueConfig());
            std::bitset<15> bits(ui_->lineEdit_ch1_value_bits->text().toStdString());
            m.SetData(static_cast<int>(bits.to_ulong()), Ch1ValueSign());
            m.Decrement(1);
            DisplayChannel1Value(m);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::AddressValueUpdated() {
    try {
        std::bitset<8> bits(ui_->lineEdit_address_value->text().toInt());
        hw_nsingle::AddressRegister a(bits);
        DisplayAddressSetPoint(a);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::AddressValueBitSetUpdated() {
    try {
        std::bitset<8> bits(ui_->lineEdit_address_value_bits->text().toStdString());
        hw_nsingle::AddressRegister a(bits);
        DisplayAddressSetPoint(a);
    }
    catch (std::exception& exc) {
        QMessageBox::warning(this, "Error", exc.what());
    }
}

void MultiplexedNSingleSimulator::DisplayAddressValue(const hw_nsingle::AddressRegister& address) {
    ui_->lineEdit_address_value_bits->setText(QString::fromStdString(address.bitset().to_string()));
    ui_->lineEdit_address_value->setText(QString::number(address.bitset().to_ulong()));
}

void MultiplexedNSingleSimulator::DisplayAddressSetPoint(const hw_nsingle::AddressRegister& address) {
    ui_->lineEdit_address_setPoint_bits->setText(QString::fromStdString(address.bitset().to_string()));
    ui_->lineEdit_address_setPoint->setText(QString::number(address.bitset().to_ulong()));
}

void MultiplexedNSingleSimulator::DisplayChannel1Value(const hw_nsingle::Measurement& m) {
    const std::bitset<15> bits(m.RawValue());
    ui_->lineEdit_ch1_value_bits->setText(QString::fromStdString(bits.to_string()));
    ui_->lineEdit_ch1_value->setText(QString::number(m.InterpretedValue(), 'f', 2));
    ui_->pushButton_ch1_value_sign->setText(SignToString(m.sign()));
}

void MultiplexedNSingleSimulator::DisplayChannel1SetPoint(const hw_nsingle::Measurement& m) {
    const std::bitset<15> bits(m.RawValue());
    ui_->lineEdit_ch1_setpoint_bits->setText(QString::fromStdString(bits.to_string()));
    ui_->lineEdit_ch1_setpoint->setText(QString::number(m.InterpretedValue(), 'f', 1));
    ui_->label_ch1_setpoint_sign->setText(SignToString(m.sign()));
}

void MultiplexedNSingleSimulator::DisplayChannel2Value(const hw_nsingle::Measurement& m) {
    const std::bitset<15> bits(m.RawValue());
    ui_->lineEdit_ch2_value_bits->setText(QString::fromStdString(bits.to_string()));
    ui_->lineEdit_ch2_value->setText(QString::number(m.InterpretedValue(), 'f', 1));
    ui_->label_ch2_value_sign->setText(SignToString(m.sign()));
}

} // namespace
