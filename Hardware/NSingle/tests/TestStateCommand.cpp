#include "TestStateCommand.h"
#include "Error.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

void TestStateCommand::initTestCase() {}

void TestStateCommand::cleanupTestCase() {}

void TestStateCommand::init() {}

void TestStateCommand::cleanup() {}

void TestStateCommand::TestSteadyStateCommandsOnDifferentBits() {
    StateCommandConfig s(false, nsingle::Bit(true, true, 1),
                                nsingle::Bit(true, true, 2),
                                nsingle::Bit(true, true, 3),
                                nsingle::Bit(true, true, 4));
    StateCommand cmd1(s);

    std::vector<QString> off = cmd1.CommandStrings(StateCommandType::Off);
    QCOMPARE(static_cast<int>(off.size()), 1);
    QCOMPARE(off.at(0), QString("04"));

    std::vector<QString> on = cmd1.CommandStrings(StateCommandType::On);
    QCOMPARE(static_cast<int>(on.size()), 1);
    QCOMPARE(on.at(0), QString("0A"));

    std::vector<QString> stdby = cmd1.CommandStrings(StateCommandType::StdBy);
    QCOMPARE(static_cast<int>(stdby.size()), 1);
    QCOMPARE(stdby.at(0), QString("02"));

    std::vector<QString> reset = cmd1.CommandStrings(StateCommandType::Reset);
    QCOMPARE(static_cast<int>(reset.size()), 1);
    QCOMPARE(reset.at(0), QString("10"));
}

void TestStateCommand::TestSteadyStateCommandsOneDisabled() {
    StateCommandConfig s(false, nsingle::Bit(false, false, 0),
                                nsingle::Bit(true, true, 1),
                                nsingle::Bit(true, true, 3),
                                nsingle::Bit(true, true, 4));
    StateCommand cmd1(s);

    std::vector<QString> off = cmd1.CommandStrings(StateCommandType::Off);
    QCOMPARE(static_cast<int>(off.size()), 1);
    QCOMPARE(off.at(0), QString("02"));

    std::vector<QString> on = cmd1.CommandStrings(StateCommandType::On);
    QCOMPARE(static_cast<int>(on.size()), 1);
    QCOMPARE(on.at(0), QString("08"));
}

void TestStateCommand::TestSteadyStateCommandsOnSameBits() {
    StateCommandConfig s(false, nsingle::Bit(true, true, 1),
                                nsingle::Bit(true, false, 2),
                                nsingle::Bit(true, true, 2),
                                nsingle::Bit(true, true, 4));
    StateCommand cmd1(s);

    std::vector<QString> off = cmd1.CommandStrings(StateCommandType::Off);
    QCOMPARE(static_cast<int>(off.size()), 1);
    QCOMPARE(off.at(0), QString("00"));

    std::vector<QString> on = cmd1.CommandStrings(StateCommandType::On);
    QCOMPARE(static_cast<int>(on.size()), 1);
    QCOMPARE(on.at(0), QString("06"));
}

void TestStateCommand::TestImpulsCommands() {
    bool impuls(true);
    StateCommandConfig s(impuls, nsingle::Bit(true, true, 1),
                                 nsingle::Bit(true, true, 2),
                                 nsingle::Bit(true, true, 3),
                                 nsingle::Bit(true, true, 4));
    StateCommand cmd1(s);

    std::vector<QString> off = cmd1.CommandStrings(StateCommandType::Off);
    QCOMPARE(static_cast<int>(off.size()), 2);
    QCOMPARE(off.at(0), QString("04"));
    QCOMPARE(off.at(1), QString("00"));

    std::vector<QString> on = cmd1.CommandStrings(StateCommandType::On);
    QCOMPARE(static_cast<int>(on.size()), 2);
    QCOMPARE(on.at(0), QString("08"));
    QCOMPARE(on.at(1), QString("00"));

    std::vector<QString> stdby = cmd1.CommandStrings(StateCommandType::StdBy);
    QCOMPARE(static_cast<int>(stdby.size()), 2);
    QCOMPARE(stdby.at(0), QString("02"));
    QCOMPARE(stdby.at(1), QString("00"));

    std::vector<QString> reset = cmd1.CommandStrings(StateCommandType::Reset);
    QCOMPARE(static_cast<int>(reset.size()), 2);
    QCOMPARE(reset.at(0), QString("10"));
    QCOMPARE(reset.at(1), QString("00"));
}

void TestStateCommand::TestImpulsCommandsNegativeLogic() {
    bool impuls(true);
    StateCommandConfig s(impuls, nsingle::Bit(true, true, 1),
                                 nsingle::Bit(true, false, 2),
                                 nsingle::Bit(true, true, 3),
                                 nsingle::Bit(true, true, 4));
    StateCommand cmd1(s);

    std::vector<QString> off = cmd1.CommandStrings(StateCommandType::Off);
    QCOMPARE(static_cast<int>(off.size()), 2);
    QCOMPARE(off.at(0), QString("00"));
    QCOMPARE(off.at(1), QString("04"));
}

}
