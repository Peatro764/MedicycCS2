#include "NodeAddressCommand.h"

#include <QDebug>
#include <math.h>

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

NodeAddressRequest::NodeAddressRequest(uint8_t client_node_address)
    : client_node_address_(client_node_address)
{
}

NodeAddressRequest::NodeAddressRequest() {
}

QByteArray NodeAddressRequest::Command() const {
    QByteArray cmd_block = AsByteArray(static_cast<uint32_t>(CMD_TYPE), 4);
    QByteArray client_node_addr = AsByteArray(static_cast<uint32_t>(client_node_address_), 4);
    QByteArray body = cmd_block + EmptyErrorBlock() + client_node_addr;
    return (FINS_TCP_HEADER + AsByteArray(static_cast<uint32_t>(body.size()), 4) + body);
}

NodeAddressReply::NodeAddressReply(QByteArray reply) {
    if (!IsValidFINSReply(reply)) {
        throw std::runtime_error("This is not a valid fins reply");
    }
    if (ParseFINSReplyType(reply) != FINS_COMMAND_TYPE::NODE_ADDRESS_RECV) {
        throw std::runtime_error("This is not a node address reply");
    }
    if (reply.size() != CMD_SIZE) {
         throw std::runtime_error("This is not a node address reply");
    }
    const uint32_t error_code = ParseFINSErrorCode(reply);
    if (error_code) {
        QString msg = QString("FINS reply contained the error: ") + QString::number(error_code);
        throw std::runtime_error(msg.toStdString());
    }
    reply_ = reply;
}

uint8_t NodeAddressReply::ClientNodeAddress() const {
    return static_cast<uint8_t>(AsInt32(reply_.mid(16, 4)));
}

uint8_t NodeAddressReply::ServerNodeAddress() const {
    return static_cast<uint8_t>(AsInt32(reply_.mid(20, 4)));
}

}

