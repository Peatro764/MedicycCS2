#ifndef COMMEQUIPMENT_FRAMESENDCOMMAND_H
#define COMMEQUIPMENT_FRAMESENDCOMMAND_H

#include "Omron_global.h"

#include <QObject>
#include <QByteArray>

#include "FINS.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    class FrameSendCommand {
    public:
        static const FINS_COMMAND_TYPE BASE_TYPE = FINS_COMMAND_TYPE::FRAME_SEND;
        FrameSendCommand() {}
        virtual ~FrameSendCommand() {}
        QByteArray Construct_FINS_FRAME_SEND(FINSFrame frame) const;
    private:
    };

}
#endif
