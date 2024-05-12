#ifndef COMMAND_CREATOR_H
#define COMMAND_CREATOR_H

#include <QObject>
#include <QString>
#include <queue>

#include "NSingleUtil.h"
#include "Config.h"
#include "Measurement.h"
#include "Error.h"
#include "StateReply.h"
#include "AddressRegister.h"
#include "StateCommand.h"
#include "Command.h"
#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class CommandCreator
{
public:
    CommandCreator(NSingleConfig config);
    ~CommandCreator();

public slots:
    std::queue<Command> On() const { return ChangeState(StateCommandType::On); }
    std::queue<Command> Off() const { return ChangeState(StateCommandType::Off); }
    std::queue<Command> Reset() const { return ChangeState(StateCommandType::Reset); }
    std::queue<Command> ChangeState(StateCommandType type) const;

    Command WriteAddressSetPoint(std::bitset<8> value) const;
    Command ReadAddressSetPoint() const;
    std::queue<Command> WriteAndVerifyAddressSetPoint(std::bitset<8> value) const;
    Command ReadAddress() const;

    std::queue<Command> WriteAndVerifyChannel1SetPoint(const Measurement& m) const;
    Command WriteChannel1SetPoint(const Measurement& m) const;
    Command WriteChannel1SetPoint(int raw_value, bool sign) const;
    Command WriteChannel1SetPoint(double interpreted_value, bool sign) const;
    Command WriteChannel1SetPoint(std::bitset<16> data) const;

    std::queue<Command> Ping() const;
    Command ReadChannel1SetPoint() const;
    Command ReadChannel1() const;
    std::queue<Command> ReadChannel1(int number) const;

    Command WriteChannel1Tolerance(std::bitset<16> value) const;

    Command ReadChannel2() const;

    Command Write(Config config) const;
    Command ReadConfig() const; // Register Z (reads all config parameters)
    Command ReadState() const;  // Register C
    Command ReadError() const; // Register X

private:
    const QByteArray cmd_footer_ = "*\r\n";
    NSingleConfig config_;

};

} // namespace

#endif
