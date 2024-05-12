#include "Command.h"

#include "ADCUtil.h"

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

Command::Command(QByteArray header, QByteArray body, QByteArray footer) :
    header_(header.toUpper()), body_(body.toUpper()), footer_(footer.toUpper()) {}

QByteArray Command::Packaged() const {
    return (header_ + body_ + util::CheckSum(header_ + body_) + footer_);
}

}
