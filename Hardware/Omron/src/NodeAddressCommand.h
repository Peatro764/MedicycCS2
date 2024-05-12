#ifndef COMMEQUIPMENT_NODEADDRESSCOMMAND_H
#define COMMEQUIPMENT_NODEADDRESSCOMMAND_H

#include "Omron_global.h"

#include <QObject>
#include <QByteArray>

#include "FINS.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

    class NodeAddressRequest {
    public:

        explicit NodeAddressRequest(uint8_t client_node_address);
        explicit NodeAddressRequest();
        ~NodeAddressRequest() {}
        QByteArray Command() const;

    private:
        uint8_t client_node_address_ = 0; // if sending 0, the server will give you a node address of his choice
        const FINS_COMMAND_TYPE CMD_TYPE = FINS_COMMAND_TYPE::NODE_ADDRESS_SEND;
    };

    class NodeAddressReply {
    public:

        explicit NodeAddressReply(QByteArray reply);
        ~NodeAddressReply() {}
        QByteArray Command() const;
        uint8_t ClientNodeAddress() const;
        uint8_t ServerNodeAddress() const;

    private:
        const FINS_COMMAND_TYPE CMD_TYPE = FINS_COMMAND_TYPE::NODE_ADDRESS_RECV;
        const int CMD_SIZE = 24;
        QByteArray reply_;
    };
}
#endif
