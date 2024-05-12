#include "ReplyParser.h"

#include "Reply.h"

#include <sstream>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

ReplyParser::ReplyParser(NSingleConfig config)
    : config_(config)
{}

ReplyParser::~ReplyParser() {
}


void ReplyParser::Parse(QByteArray data) {
    try {
        QString data_string(data);
        Reply reply(data_string);
        if (!reply.IsValid() || reply.Type() == ReplyType::UNKNOWN) {
            qWarning() << "ReplyParser::Parse Received non valid or unknown reply type";
            emit SIGNAL_ParseError("Reçu un paquet de données inconnu de nsingle: " + data);
            return;
        }
        // The ReplyReceived should be sent out before any processed replies (convention)
        emit SIGNAL_ReplyReceived();

        switch (reply.Type()) {
        case ReplyType::A:
            qWarning() << "ReplyParser::Parse NOT IMPLEMENTED COMMAND REPLY RECEIVED (A)";
            break;
        case ReplyType::E:
            ProcessAddressSetPoint(reply.Body());
            break;
        case ReplyType::F:
            ProcessAddressValue(reply.Body());
            break;
        case ReplyType::B:
            ProcessChannel1SetPoint(reply.Body());
            break;
        case ReplyType::D:
            ProcessChannel1Value(reply.Body());
            break;
        case ReplyType::G:
            ProcessChannel2Value(reply.Body());
            break;
        case ReplyType::C:
            ProcessStateChange(reply.Body());
            break;
        case ReplyType::X:
            ProcessError(reply.Body());
            break;
        case ReplyType::Z:
            ProcessConfig(reply.Body());
            break;
        case ReplyType::UNKNOWN:
            qWarning() << "ReplyParser::Parse Unknown reply type";
            break;
        default:
            qWarning() << "ReplyParser::Parse Ignored reply: " << data;
            break;
        }
    } catch (std::exception& exc) {
        emit SIGNAL_ParseError(exc.what());
    }
}

void ReplyParser::ProcessConfig(QString body) {
    Config config(body);
    emit SIGNAL_ConfigUpdated(config);
}

void ReplyParser::ProcessStateChange(QString body) {
    StateReply state_reply(config_.GetStateReplyConfig());
    state_reply.SetData(body);
    emit SIGNAL_StateUpdated(state_reply);
    if (state_reply.Channel(StateReplyChannel::On)) emit SIGNAL_On();
    if (state_reply.Channel(StateReplyChannel::Off)) emit SIGNAL_Off();
    if (state_reply.Channel(StateReplyChannel::Error)) {
        emit SIGNAL_Error();
    } else {
        emit SIGNAL_Ok();
    }
    if (state_reply.Channel(StateReplyChannel::Local)) emit SIGNAL_Local();
    if (state_reply.Channel(StateReplyChannel::Remote)) emit SIGNAL_Remote();
}

void ReplyParser::ProcessError(QString body) {
    Error error(body);
    emit SIGNAL_ErrorUpdated(error);
}

void ReplyParser::ProcessAddressSetPoint(QString body) {
    AddressRegister address(body);
    emit SIGNAL_AddressSetPoint(address);
}

void ReplyParser::ProcessAddressValue(QString body) {
    AddressRegister address(body);
    emit SIGNAL_AddressValue(address);
}

void ReplyParser::ProcessChannel1SetPoint(QString body) {
    Measurement m(config_.GetChannel1SetPointConfig());
    if (m.SetData(body)) {
        emit SIGNAL_Channel1SetPoint(m);
        emit SIGNAL_Channel1SetPoint(m.InterpretedValue(), m.sign());
    } else {
        QString message = QString("La valeur de consigne lue ne rentre pas dans le registre nsingle: %1").arg(body);
        throw std::runtime_error(message.toStdString());
    }
}

void ReplyParser::ProcessChannel1Value(QString body) {
    Measurement m(config_.GetChannel1ValueConfig());
    if (m.SetData(body)) {
        qDebug() << "ReplyParser::ProcessChannel1Value HEX " << m.HexValue() << " Physical value " << m.InterpretedValue();
        emit SIGNAL_Channel1Value(m);
        emit SIGNAL_Channel1Value(m.InterpretedValue(), m.sign());
    } else {
        QString message = QString("La valeur lue ne rentre pas dans le registre nsingle: %1").arg(body);
        throw std::runtime_error(message.toStdString());
    }
}

void ReplyParser::ProcessChannel2Value(QString body) {
    Measurement m(config_.GetChannel1ValueConfig());
    if (m.SetData(body)) {
        emit SIGNAL_Channel2Value(m);
    } else {
        QString message = QString("La valeur lue ne rentre pas dans le registre nsingle: %1").arg(body);
        throw std::runtime_error(message.toStdString());
    }
}

} // ns
