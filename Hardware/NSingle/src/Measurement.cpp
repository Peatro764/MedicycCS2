#include "Measurement.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

Measurement::Measurement() {}

Measurement::Measurement(MeasurementConfig config)
 : config_(config) {}


bool Measurement::CanContainData(double abs_interpreted_value) const {
    if (abs_interpreted_value < 0.0) {
        return false;
    }
    int abs_raw_value = static_cast<int>(std::round(abs_interpreted_value / config_.bit_value()));
    return CanContainData(abs_raw_value);
}

bool Measurement::CanContainData(int abs_raw_value) const {
    if (abs_raw_value < 0) {
        return false;
    }
    return (abs_raw_value <= config_.max());
}

bool Measurement::CanContainData(QString four_ch_hex) const {
    if (four_ch_hex.size() != 4) {
        return false;
    }
    bool ok(false);
    int dec = four_ch_hex.toUInt(&ok, 16);
    dec &= ~(1UL << config_.sign_bit()); // remove sign bit
    if (!ok) {
        return false;
    }
    return CanContainData(dec);
}

bool Measurement::SetData(double abs_interpreted_value, bool sign) {
    if (CanContainData(abs_interpreted_value)) {
        const int abs_raw_value = static_cast<int>(std::round(abs_interpreted_value / config_.bit_value()));
        SetData(abs_raw_value, sign);
        return true;
    } else {
        return false;
    }
}

bool Measurement::SetData(int abs_raw_value, bool sign) {
    if (CanContainData(abs_raw_value)) {
       //    if (abs_raw_value < config_.min()) {
       //        qDebug() << "Measurement::SetData Value too small. Set value = " << abs_raw_value << " min = " << config_.min();
       //        throw std::runtime_error("The register cannot hold a value with of the requested size (too small)");
       //    }
      // In the nsingle register, negative polarity is 1
      data_ = (abs_raw_value & config_.max()) + ((sign ? 0 : 1) << config_.sign_bit());
      return true;
    } else {
        return false;
    }
}

bool Measurement::SetData(QString hex) {
    if (CanContainData(hex)) {
        bool ok(false);
        quint16 dec = hex.toUInt(&ok, 16); // ok checked in CanContainData
        int sign = (0x1 << config_.sign_bit()) & dec;
        int value = (dec & config_.max());
        data_ = (value + sign);
        return true;
    } else {
        return false;
    }
}

bool Measurement::SetData(std::bitset<16> bits) {
    if (CanContainData(static_cast<int>(bits.to_ulong()))) {
        data_ = bits.to_ulong();
        return true;
    } else {
        return false;
    }
}

void Measurement::Decrement(int nbits) {
    if (nbits < 0) {
        qWarning() << "Measurement::Decrement Negative number of bits";
        return;
    }
    const int decr = nbits * (0x1 << config_.lsb_bit());
    if (static_cast<int32_t>(data_ & config_.max()) - static_cast<int32_t>(decr) >= 0) {
        data_ -= decr;
    } else {
        data_ &= ~config_.max();
    }
}

void Measurement::Increment(int nbits) {
    if (nbits < 0) {
        qWarning() << "Measurement::Increment Negative number of bits";
        return;
    }
    const int32_t incr = nbits * (0x1 << config_.lsb_bit());
    if (incr + static_cast<int32_t>(data_ & config_.max()) <= config_.max()) {
        data_ += incr;
    } else {
        data_ |= config_.max();
    }
}

void Measurement::InvertSign() {
    SetData(RawValue(), !sign());
}

double Measurement::InterpretedValue() const {
    return config_.bit_value() * RawValue();
}

int Measurement::RawValue() const {
//    const int shifted = data_ >> config_.lsb_bit();
    // Mask non used bits
    return data_ & config_.max();
}

int Measurement::OneBit() const {
    return config_.min();
}

QString Measurement::HexValue() const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << data_;
    return QString("%1").arg(QString(buffer.toHex()), 4, QLatin1Char('0')).toUpper();
}

bool Measurement::sign() const {
    // In nsingle, negative polarity is bit value = 1
    return ((0x1 & (data_ >> config_.sign_bit())) == 0);
}

bool Measurement::inverted_sign() const {
    return !sign();
}

std::bitset<16> Measurement::bits() const {
    return std::bitset<16>(data_);
}

bool operator==(const MeasurementConfig &c1, const MeasurementConfig& c2) {
    return (c1.lsb_bit() == c2.lsb_bit() &&
            c1.msb_bit() == c2.msb_bit() &&
            c1.sign_bit() == c2.sign_bit() &&
            std::abs(c1.bit_value() - c2.bit_value()) < 0.00001);
}

bool operator!=(const MeasurementConfig &c1, const MeasurementConfig& c2) {
    return !(c1 == c2);
}

bool operator==(const Measurement &c1, const Measurement& c2) {
    return (c1.config_ == c2.config_ &&
            (c1.data_ & c1.config_.max()) == (c2.data_ & c2.config_.max()));
}

bool operator!=(const Measurement &c1, const Measurement& c2) {
    return !(c1 == c2);
}

} // ns
