#include "MonitoredState.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::global {

MonitoredState::MonitoredState(QState *parent, int timeout_ms, QString message, int max_tries, QAbstractState *error_state)
    : QState(parent), max_tries_(max_tries) {
    timeout_timer_.setInterval(timeout_ms);
    timeout_timer_.setSingleShot(false);

    QObject::connect(&timeout_timer_, &QTimer::timeout, this, [&, message]() { emit SIGNAL_Timeout(message); });
    QObject::connect(&timeout_timer_, &QTimer::timeout, this, &MonitoredState::IncrementCounter);
    this->addTransition(this, &MonitoredState::SIGNAL_Bailout, error_state);
}

void MonitoredState::IncrementCounter() {
    if (++n_tries_ < max_tries_) {
        emit SIGNAL_DoWork();
    } else {
        emit SIGNAL_Bailout("Le nombre maximum d'essais a été effectué, procédure arreté");
    }
}

void MonitoredState::onEntry(QEvent *event) {
//    qDebug() << "MonitoredState::onEntry";
    (void)event;
    n_tries_ = 0;
    emit SIGNAL_DoWork();
    timeout_timer_.start();
}

void MonitoredState::onExit(QEvent *event) {
//    qDebug() << "MonitoredState::onExit";
    (void)event;
    timeout_timer_.stop();
}


}
