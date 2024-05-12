#ifndef COMMAND_H
#define COMMAND_H

#include <QString>

#include "SocketClient.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

class Command {
public:
    Command();
    Command(bool wait_confirmation, bool wait_reply);
    Command(QString body);
    Command(bool wait_confirmation, bool wait_reply, QString body);
    QString ascii() const;
    bool wait_confirmation() const;
    bool wait_reply() const;
    void Send(SocketClient *socket);
protected:
    bool wait_confirmation_ = true;
    bool wait_reply_ = true;
    QString body_ = "";
    QString footer_ = "\r\n";
};

}

#endif
