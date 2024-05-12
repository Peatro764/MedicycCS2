#ifndef REPLY_H
#define REPLY_H

#include <QObject>
#include <QString>
#include <QChar>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

enum class ReplyType {A, B, C, D, E, F, G, H, I, J, K, L, M, N, UNKNOWN};

class Reply
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

}

#endif
