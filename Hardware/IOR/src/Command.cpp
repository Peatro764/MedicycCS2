#include "Command.h"

#include "IORUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

Command::Command(QString header, QString body, QString footer) :
    header_(header.toUpper()), body_(body.toUpper()), footer_(footer.toUpper()) {}

QString Command::Packaged() const {
    return (header_ + body_ + ior_util::CheckSum(header_ + body_) + footer_);
}

}
