#ifndef ADC_COMMAND_H
#define ADC_COMMAND_H

#include "ADC_global.h"

#include <QObject>
#include <QByteArray>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class Command
{
public:
    Command(QByteArray header, QByteArray body, QByteArray footer);
    QByteArray Packaged() const;

private:
    QByteArray header_;
    QByteArray body_;
    QByteArray footer_;
};

}

#endif
