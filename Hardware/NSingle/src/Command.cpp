#include "Command.h"

#include "NSingleUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

Command::Command(QString header, QString body, QString footer) :
    header_(header.toUpper()), body_(body.toUpper()), footer_(footer.toUpper()), reply_(true), execution_time_(0) {}

Command::Command(QString header, QString body, QString footer, int execution_time) :
    header_(header.toUpper()), body_(body.toUpper()), footer_(footer.toUpper()), reply_(false), execution_time_(execution_time) {}

QString Command::Packaged() const {
    return (header_ + body_ + util::CheckSum(header_ + body_) + footer_);
}

} // ns
