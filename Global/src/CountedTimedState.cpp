#include "CountedTimedState.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::global {

CountedTimedState::CountedTimedState(QState *parent, int max_ncounts, QString count_message, int timeout_ms, QString timeout_message)
    : QState(parent),
      max_ncounts_(max_ncounts), count_message_(count_message), timeout_ms_(timeout_ms), timeout_message_(timeout_message)
{
    timeout_timer_.setInterval(timeout_ms);
    timeout_timer_.setSingleShot(true);

    QObject::connect(&timeout_timer_, &QTimer::timeout, this, [&]() { emit SIGNAL_Timeout(timeout_message_); });
}

void CountedTimedState::onEntry(QEvent *event) {
//    qDebug() << "CountedTimedState::onEntry";
    (void)event;
    timeout_timer_.start();
}

void CountedTimedState::onExit(QEvent *event) {
//    qDebug() << "CountedTimedState::onExit";
    (void)event;
    timeout_timer_.stop();
}

void CountedTimedState::Increment() {
    if (counter_++ >= max_ncounts_) {
        emit SIGNAL_ThresholdReached(count_message_);
    }
}

void CountedTimedState::Reset() {
    counter_ = 0;
}


}
