#ifndef NSINGLE_STATECOMMAND_H
#define NSINGLE_STATECOMMAND_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <bitset>
#include <QMap>
#include <QVariant>

#include "NSingleUtil.h"
#include "Bit.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

enum class StateCommandType { StdBy = 0, Off = 1, On = 2, Reset = 3 };

class NSINGLE_EXPORT StateCommandConfig {
public:
    StateCommandConfig(bool impuls_commands,
                       Bit stdby, Bit off, Bit on, Bit reset) {
        impuls_ = impuls_commands;
        bits_[StateCommandType::StdBy] = stdby;
        bits_[StateCommandType::Off] = off;
        bits_[StateCommandType::On] = on;
        bits_[StateCommandType::Reset] = reset;
    }

    int bit(StateCommandType type) const { return bits_[type].index(); }
    bool logic(StateCommandType type) const { return bits_[type].logic(); }
    bool enabled(StateCommandType type) const { return bits_[type].enabled(); }
    bool impuls() const { return impuls_; }
private:
    friend bool operator==(const StateCommandConfig &c1, const StateCommandConfig& c2);
    bool impuls_;
    QMap<StateCommandType, Bit> bits_;
};

bool operator==(const StateCommandConfig &c1, const StateCommandConfig& c2);
bool operator!=(const StateCommandConfig &c1, const StateCommandConfig& c2);

class StateCommand : public QObject
{
    Q_OBJECT

public:
    StateCommand(StateCommandConfig config);
    std::vector<QString> CommandStrings(StateCommandType type) const;

private:
    friend bool operator==(const StateCommand &c1, const StateCommand& c2);
    std::bitset<8> PositiveFlankCommand(StateCommandType type) const;
    std::bitset<8> NegativeFlankCommand() const;
    QString hexcode(std::bitset<8> bits) const;
    StateCommandConfig config_;
};

bool operator==(const StateCommand &c1, const StateCommand& c2);
bool operator!=(const StateCommand &c1, const StateCommand& c2);

} // namespace

#endif
