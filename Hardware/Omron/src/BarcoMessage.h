#ifndef BARCOMESSAGE_H
#define BARCOMESSAGE_H

#include <QObject>
#include <QDebug>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class BarcoMessage {
public:
    enum class SEVERITY { INFO = 0, WARNING = 1, ERROR = 2}; // to match db table BarcoMessage
    BarcoMessage();
    BarcoMessage(int id, SEVERITY severity, QString message, bool enabled);
    SEVERITY severity() const { return severity_; }
    int id() const { return id_; }
    QString message() const { return message_; }
    bool enabled() const { return enabled_ ;}


private:
    int id_;
    SEVERITY severity_;
    QString message_;
    bool enabled_;
};

bool operator==(const BarcoMessage &c1, const BarcoMessage& c2);
bool operator!=(const BarcoMessage &c1, const BarcoMessage& c2);

}

#endif

