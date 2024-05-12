#ifndef NSINGLE_MEASUREMENT_H
#define NSINGLE_MEASUREMENT_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT MeasurementConfig {
public:
    MeasurementConfig() {}
    MeasurementConfig(int lsb_bit, int msb_bit, int sign_bit, double bit_value)
        : lsb_bit_(lsb_bit), msb_bit_(msb_bit), sign_bit_(sign_bit), bit_value_(bit_value) {
        if (msb_bit <= lsb_bit) {
            throw std::runtime_error("Measurement config corrupt, msb bit <= lsb bit");
        }
        if (msb_bit > 15 || lsb_bit < 0) {
            throw std::runtime_error("Measurement config corrupt, msb or lsb bit out of range");
        }
        if (!((sign_bit > msb_bit || sign_bit < lsb_bit) && sign_bit >= 0 && sign_bit <= 15)) {
            throw std::runtime_error("Measurement config corrupt, sign bit out of range");
        }
    }
    int n_value_bits() const { return msb_bit() + 1; }
    int lsb_bit() const { return lsb_bit_; }
    int msb_bit() const { return msb_bit_; }
    int sign_bit() const { return sign_bit_; }
    double bit_value() const  { return bit_value_; }
    int min() const { return std::pow(2, lsb_bit_); }
    int max() const { return std::pow(2, msb_bit_ + 1) - 1 - (min() - 1); }
    // the upperlimit functions dont takes into account bits lower than lsbbit,
    // and thus gives a slightly overestimated max for registers that dont start at bit 0 (some dont)
    int upperlimit() const { return std::pow(2, msb_bit_ + 1) - 1; }


private:
    int lsb_bit_;
    int msb_bit_;
    int sign_bit_;
    double bit_value_;
};

bool operator==(const MeasurementConfig &c1, const MeasurementConfig& c2);
bool operator!=(const MeasurementConfig &c1, const MeasurementConfig& c2);

class NSINGLE_EXPORT Measurement
{
    Q_GADGET

public:
    Measurement();
    Measurement(MeasurementConfig config);
    bool CanContainData(double abs_interpreted_value) const;
    bool CanContainData(int abs_raw_value) const;
    bool CanContainData(QString four_ch_hex) const;
    bool SetData(double abs_interpreted_value, bool sign);
    bool SetData(int abs_raw_value, bool sign);
    bool SetData(QString hex);
    bool SetData(std::bitset<16> bits);
    void Decrement(int nbits);
    void Increment(int nbits);
    void InvertSign();
    double InterpretedValue() const;
    int RawValue() const;
    int OneBit() const;
    QString HexValue() const;
    bool sign() const;
    bool inverted_sign() const;
    std::bitset<16> bits() const;

private:
    friend bool operator==(const Measurement &c1, const Measurement& c2);
    MeasurementConfig config_;
    quint16 data_ = 0;
};

bool operator==(const Measurement &c1, const Measurement& c2);
bool operator!=(const Measurement &c1, const Measurement& c2);

} // namespace

#endif
