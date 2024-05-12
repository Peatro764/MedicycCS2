#ifndef ADC_CHANNEL_H
#define ADC_CHANNEL_H

#include "ADC_global.h"

#include <QObject>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class ADC_EXPORT Channel {
public:
    Channel();
    Channel(QString name, int card, int address, double conversion_factor, QString unit, double sim_value, double sim_step);
    QString Name() const { return name_; }
    int Card() const { return card_; }
    int Address() const { return address_; }
    double ConversionFactor() const { return conversion_factor_; }
    QString Unit() const { return unit_; }
    QByteArray Hex(int bit_value) const { return QString("%1").arg(bit_value, 3, 16, QLatin1Char( '0' )).toUpper().toLatin1(); }
    double ADCValue(int bit_value) const { return bit_value * 0.002442; }
    double PhysicalValue(int bit_value) const { return bit_value * conversion_factor_; }
    double SimValue() const { return sim_value_; }
    double SimStep() const { return sim_step_; }

private:
    QString name_;
    int card_;
    int address_;
    double conversion_factor_;
    QString unit_;
    double sim_value_;
    double sim_step_;
};

bool operator==(const Channel &c1, const Channel& c2);
bool operator!=(const Channel &c1, const Channel& c2);

}

#endif // CHANNEL_H
