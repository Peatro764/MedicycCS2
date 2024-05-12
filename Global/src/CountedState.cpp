#include "CountedState.h"

#include <QDebug>
#include <iostream>

namespace medicyc::cyclotroncontrolsystem::global {

CountedState::CountedState(QState *parent, int max_ncounts, QString message)
    : QState(parent), max_ncounts_(max_ncounts), message_(message) {
}

void CountedState::onEntry(QEvent *event) {
    (void)event;
}

void CountedState::onExit(QEvent *event) {
    (void)event;
}

void CountedState::Increment() {
    if (counter_++ >= max_ncounts_) {
        emit SIGNAL_ThresholdReached(message_);
    }
    std::cout << "Counter " << counter_ << std::endl;
}

void CountedState::Reset() {
    counter_ = 0;
}

}
