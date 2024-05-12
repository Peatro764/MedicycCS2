#ifndef ADC_REPLY_H
#define ADC_REPLY_H

#include "ADC_global.h"

#include <QObject>
#include <QByteArray>
#include <QChar>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

enum class ReplyType {A, E, F, G, H, I, J, K, L, U, UNKNOWN};

class Reply
{
public:
    Reply(QByteArray data);
    bool IsValid() const;
    QByteArray Header() const;
    ReplyType Type() const;
    QByteArray Body() const;
    QByteArray Footer() const;
    QByteArray CheckSum() const;

private:
    bool CorrectSize() const;
    QByteArray data_;
};

}

#endif
