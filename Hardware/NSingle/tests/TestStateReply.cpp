#include "TestStateReply.h"
#include "Error.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

StateReplyConfig TestStateReply::GetStateReplyConfig() {
    StateReplyConfig s(Bit(true, true, 1),
                       Bit(true, false, 0),
                       Bit(true, true, 2),
                       Bit(true, true, 3),
                       Bit(true, true, 7),
                       Bit(true, false, 4),
                       Bit(true, true, 5),
                       Bit(true, true, 6),
                       Bit(true, true, 7));
    return s;
}

void TestStateReply::initTestCase() {}

void TestStateReply::cleanupTestCase() {}

void TestStateReply::init() {}

void TestStateReply::cleanup() {}

void TestStateReply::TestSetGetData() {
    StateReply state(GetStateReplyConfig());

    state.SetData(QString("3A"));
    QCOMPARE(state.hexcode(), QString("3A"));
    std::bitset<8> bits (0x3A);
    QCOMPARE(state.bitset(), bits);

    state.SetData(bits);
    QCOMPARE(state.hexcode(), QString("3A"));
    QCOMPARE(state.bitset(), bits);
}

void TestStateReply::TestChannel() {
    StateReply state(GetStateReplyConfig());

    state.SetData(QString("3A"));
    QCOMPARE(state.Channel(StateReplyChannel::Error), false);
    QCOMPARE(state.Channel(StateReplyChannel::Local), false);
    QCOMPARE(state.Channel(StateReplyChannel::Off), true);
    QCOMPARE(state.Channel(StateReplyChannel::Ok), true);
    QCOMPARE(state.Channel(StateReplyChannel::On), false);
    QCOMPARE(state.Channel(StateReplyChannel::Remote), true);
    QCOMPARE(state.Channel(StateReplyChannel::StdBy), true);
    QCOMPARE(state.Channel(StateReplyChannel::Switch1), false);
    QCOMPARE(state.Channel(StateReplyChannel::Switch2), false);

    state.SetData(QString("C5"));
    QCOMPARE(state.Channel(StateReplyChannel::Error), true);
    QCOMPARE(state.Channel(StateReplyChannel::Local), true);
    QCOMPARE(state.Channel(StateReplyChannel::Off), false);
    QCOMPARE(state.Channel(StateReplyChannel::Ok), false);
    QCOMPARE(state.Channel(StateReplyChannel::On), true);
    QCOMPARE(state.Channel(StateReplyChannel::Remote), false);
    QCOMPARE(state.Channel(StateReplyChannel::StdBy), false);
    QCOMPARE(state.Channel(StateReplyChannel::Switch1), true);
    QCOMPARE(state.Channel(StateReplyChannel::Switch2), true);

}

}
