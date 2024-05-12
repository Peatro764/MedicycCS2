#include "Command.h"

namespace medicyc::cyclotroncontrolsystem::hardware::radiationmonitoring {

Command::Command() {}

Command::Command(bool wait_confirmation, bool wait_reply)
    : wait_confirmation_(wait_confirmation), wait_reply_(wait_reply) {}

Command::Command(QString body)
    : body_(body) {}

Command::Command(bool wait_confirmation, bool wait_reply, QString body)
    : wait_confirmation_(wait_confirmation), wait_reply_(wait_reply), body_(body) {}

QString Command::ascii() const {
    return (body_ + " " + footer_);
}

bool Command::wait_confirmation() const {
    return wait_confirmation_;
}

bool Command::wait_reply() const {
    return wait_reply_;
}

void Command::Send(SocketClient *socket) {
    socket->Write(ascii());
}

}
