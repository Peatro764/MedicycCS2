#ifndef NSINGLE_REPLY_H
#define NSINGLE_REPLY_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <QChar>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

enum class ReplyType {A, B, C, D, E, F, G, X, Z, UNKNOWN};

class NSINGLE_EXPORT Reply
{
public:
    Reply(QString data);
    bool IsValid() const;
    QString Header() const;
    ReplyType Type() const;
    QString Body() const;
    QString Footer() const;
    QString CheckSum() const;

private:
    bool CorrectSize() const;
    QString data_;
};

} // namespace

#endif
