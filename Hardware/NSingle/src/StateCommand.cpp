#include "StateCommand.h"

#include <QByteArray>
#include <QDataStream>
#include <QMetaEnum>
#include <QDebug>

#include "SocketClient.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

StateCommand::StateCommand(StateCommandConfig config)
    : config_(config) {}

std::vector<QString> StateCommand::CommandStrings(StateCommandType type) const {
    if (!config_.enabled(type)) {
        return std::vector<QString>();
    }
    std::vector<QString> commands;
    commands.push_back(hexcode(PositiveFlankCommand(type)));
    if (config_.impuls()) {
        commands.push_back(hexcode(NegativeFlankCommand()));
    }
    return commands;
}

std::bitset<8> StateCommand::PositiveFlankCommand(StateCommandType type) const {
    std::bitset<8> cmd_bits(0);
    // set all bits to zero
    if (config_.enabled(StateCommandType::Off)) cmd_bits.set(config_.bit(StateCommandType::Off), !config_.logic(StateCommandType::Off));
    if (config_.enabled(StateCommandType::StdBy)) cmd_bits.set(config_.bit(StateCommandType::StdBy), !config_.logic(StateCommandType::StdBy));
    if (config_.enabled(StateCommandType::On)) cmd_bits.set(config_.bit(StateCommandType::On), !config_.logic(StateCommandType::On));
    if (config_.enabled(StateCommandType::Reset)) cmd_bits.set(config_.bit(StateCommandType::Reset), !config_.logic(StateCommandType::Reset));
    cmd_bits.set(config_.bit(type), config_.logic(type));
    if (!config_.impuls() && (type == StateCommandType::On) && config_.enabled(StateCommandType::StdBy)) {
        cmd_bits.set(config_.bit(StateCommandType::StdBy), config_.logic(StateCommandType::StdBy));
    }
    return cmd_bits;
}

std::bitset<8> StateCommand::NegativeFlankCommand() const {
    std::bitset<8> cmd_bits(0);
    if (config_.enabled(StateCommandType::Off)) cmd_bits.set(config_.bit(StateCommandType::Off), !config_.logic(StateCommandType::Off));
    if (config_.enabled(StateCommandType::StdBy)) cmd_bits.set(config_.bit(StateCommandType::StdBy), !config_.logic(StateCommandType::StdBy));
    if (config_.enabled(StateCommandType::On)) cmd_bits.set(config_.bit(StateCommandType::On), !config_.logic(StateCommandType::On));
    if (config_.enabled(StateCommandType::Reset)) cmd_bits.set(config_.bit(StateCommandType::Reset), !config_.logic(StateCommandType::Reset));
    return cmd_bits;
}

QString StateCommand::hexcode(std::bitset<8> bits) const {
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << static_cast<quint8>(bits.to_ulong());
    return QString("%1").arg(QString(buffer.toHex()), 2, QLatin1Char('0')).toUpper();
}

bool operator==(const StateCommandConfig &c1, const StateCommandConfig& c2) {
    return (c1.impuls_ == c2.impuls_ &&
            c1.bits_ == c2.bits_);
}

bool operator!=(const StateCommandConfig &c1, const StateCommandConfig& c2) {
    return !(c1 == c2);
}

bool operator==(const StateCommand &c1, const StateCommand& c2) {
    return (c1.config_ == c2.config_);
}

bool operator!=(const StateCommand &c1, const StateCommand& c2) {
        return !(c1 == c2);
}

} // ns
