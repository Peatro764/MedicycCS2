#include "CommandCreator.h"

#include "Reply.h"

#include <queue>
#include <unistd.h>
#include <iomanip>
#include <sstream>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

CommandCreator::CommandCreator(NSingleConfig config)
    : config_(config)
{}

CommandCreator::~CommandCreator() {
}

std::queue<Command> CommandCreator::ChangeState(StateCommandType type) const {
    StateCommand stateCommand(config_.GetStateCommandConfig());
    std::vector<QString> cmdStrings = stateCommand.CommandStrings(type);
    std::queue<Command> commands;
    for (auto &str : cmdStrings) {
        Command cmd("$A=", str, cmd_footer_, 500);
        commands.push(cmd);
    }
    commands.push(this->ReadState());
    return commands;
}

// ADDRESS COMMANDS

Command CommandCreator::WriteAddressSetPoint(std::bitset<8> value) const {
    AddressRegister address(value);
    return Command("$E=", address.hexcode(), cmd_footer_, 100);
}

Command CommandCreator::ReadAddressSetPoint() const {
    return Command("$E?", "", cmd_footer_);
}

std::queue<Command> CommandCreator::WriteAndVerifyAddressSetPoint(std::bitset<8> value) const {
    AddressRegister address(value);
    std::queue<Command> commands;
    commands.push(Command("$E=", address.hexcode(), cmd_footer_, 100));
    commands.push(Command("$F?", "", cmd_footer_));
    return commands;
}

Command CommandCreator::ReadAddress() const {
    return Command("$F?", "", cmd_footer_);
}

// CHANNEL1 COMMANDS

// CAN THROW
std::queue<Command> CommandCreator::WriteAndVerifyChannel1SetPoint(const Measurement& m) const {
    if (!m.sign() && config_.GetLowLevelConfig().polarity() == Config::Polarity::Uni) {
        throw std::runtime_error("NSingle est unipolaire, ne peut pas écrire une polarité négative");
    }
    std::queue<Command> commands;
    commands.push(Command("$B=", m.HexValue(), cmd_footer_, 100));
    commands.push(Command("$B?", "", cmd_footer_));
    return commands;
}

// CAN THROW
Command CommandCreator::WriteChannel1SetPoint(const Measurement& m) const {
    if (!m.sign() && config_.GetLowLevelConfig().polarity() == Config::Polarity::Uni) {
        throw std::runtime_error("NSingle est unipolaire, ne peut pas écrire une polarité négative");
    }
    return Command("$B=", m.HexValue(), cmd_footer_, 100);
}

Command CommandCreator::WriteChannel1SetPoint(int raw_value, bool sign) const {
    Measurement m(config_.GetChannel1SetPointConfig());
    if (!m.SetData(raw_value, sign)) {
        throw std::runtime_error("Le registre ne peut pas contenir la valeur donnée");
    }
    return this->WriteChannel1SetPoint(m);
}

Command CommandCreator::WriteChannel1SetPoint(double interpreted_value, bool sign) const {
    Measurement m(config_.GetChannel1SetPointConfig());
    if (!m.SetData(interpreted_value, sign)) {
        throw std::runtime_error("Le registre ne peut pas contenir la valeur donnée");
    }
    return this->WriteChannel1SetPoint(m);
}

Command CommandCreator::WriteChannel1SetPoint(std::bitset<16> data) const {
    Measurement m(config_.GetChannel1SetPointConfig());
    if (!m.SetData(data)) {
        throw std::runtime_error("Le registre ne peut pas contenir la valeur donnée");
    }
    return Command("$B=", m.HexValue(), cmd_footer_, 100);
}

std::queue<Command> CommandCreator::Ping() const {
    std::queue<Command> commands;
    commands.push(ReadChannel1());
    commands.push(ReadState());
    return commands;
}

Command CommandCreator::ReadChannel1SetPoint() const {
    return Command("$B?", "", cmd_footer_);
}

Command CommandCreator::ReadChannel1() const {
    return Command("$D?", "", cmd_footer_);
}

std::queue<Command> CommandCreator::ReadChannel1(int number) const {
    if (number < 1) {
        throw std::runtime_error("Cannot execute less than 1 command");
    }
    std::queue<Command> commands;
    for (int it = 0; it < number; ++it) {
        commands.push(ReadChannel1());
    }
    return commands;
}

Command CommandCreator::WriteChannel1Tolerance(std::bitset<16> value) const {
//    std::bitset<16> full(value.to_ulong());
//    full.set(14, filter);
//    full.set(15, on);

    std::stringstream ss;
    ss << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << value.to_ulong();
    return Command("$X=", QString::fromStdString(ss.str()), cmd_footer_, 100);
}

// CHANNEL 2 COMMANDS

Command CommandCreator::ReadChannel2() const {
    return Command("$G?", "", cmd_footer_);
}

// STATE AND CONFIG COMMANDS

Command CommandCreator::Write(Config config) const {
    return Command("$Z=", config.hexcode(), cmd_footer_, 500);
}

Command CommandCreator::ReadState() const {
    return Command("$C?", "", cmd_footer_);
}

Command CommandCreator::ReadError() const {
    return Command("$X?", "", cmd_footer_);
}

Command CommandCreator::ReadConfig() const {
    return Command("$Z?", "", cmd_footer_);
}

} // ns
