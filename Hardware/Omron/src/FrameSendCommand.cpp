#include "FrameSendCommand.h"

#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

QByteArray FrameSendCommand::Construct_FINS_FRAME_SEND(FINSFrame frame) const {
    QByteArray cmd_block = AsByteArray(static_cast<uint32_t>(BASE_TYPE), 4);
    QByteArray body =  cmd_block + EmptyErrorBlock() + frame.toByteArray();
    return (FINS_TCP_HEADER + AsByteArray(static_cast<uint32_t>(body.size()), 4) + body);
}

}

