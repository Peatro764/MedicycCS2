#include "SoundMessages.h"

#include <QDebug>

#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace global = medicyc::cyclotroncontrolsystem::global;

SoundMessages::SoundMessages(QObject *parent)
    : QObject{parent}
{
    SetupStateMachine();
    tErrorMessageTimer.setInterval(40000);
    tErrorMessageTimer.setSingleShot(true);
}


void SoundMessages::SetupStateMachine() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        global::TimedState *sPlay = new global::TimedState(sSuperState, 60000, "Timeout in play state", sIdle);

    // SuperState
    sSuperState->setInitialState(sIdle);

        // Idle
        QObject::connect(sIdle, &QState::entered, this, &SoundMessages::CheckQueue);
        sIdle->addTransition(this, &SoundMessages::SIGNAL_MessageAdded, sPlay);

        // Play
        QTimer *tCheckPlayStatus = new QTimer(sPlay);
        tCheckPlayStatus->setInterval(300);
        tCheckPlayStatus->setSingleShot(false);
        QObject::connect(tCheckPlayStatus, &QTimer::timeout, this, &SoundMessages::CheckMessageStatus);
        QObject::connect(sPlay, &global::TimedState::SIGNAL_Timeout, this, [&](QString message) { qWarning() << message; });
        QObject::connect(sPlay, &QState::entered, this, &SoundMessages::PopQueue);
        QObject::connect(sPlay, &QState::entered, tCheckPlayStatus, qOverload<>(&QTimer::start));
        QObject::connect(sPlay, &QState::exited, tCheckPlayStatus, &QTimer::stop);
        sPlay->addTransition(this, &SoundMessages::SIGNAL_MessageFinished, sIdle);

    sm_.addState(sSuperState);
    sm_.setInitialState(sSuperState);
    sm_.start();
}

void SoundMessages::CheckMessageStatus() {
    if (sound_) {
        if (sound_->isFinished()) emit SIGNAL_MessageFinished();
    } else {
        qWarning() << "SoundMessages::CheckMessageStatus No QSound object";
    }
}

void SoundMessages::PopQueue() {
    if (messages_.empty()) {
        qWarning() << "SoundMessages::PopQueue Queue empty";
        emit SIGNAL_MessageFinished();
    }
    if (sound_) {
        delete sound_;
    }
    sound_ = new QSound(messages_.dequeue());
    sound_->play();
}


void SoundMessages::CheckQueue() {
    if (!messages_.isEmpty()) {
        emit SIGNAL_MessageAdded();
    }
}

void SoundMessages::AddErrorMessage() {
    if (tErrorMessageTimer.isActive()) {
        qWarning() << "SoundMessages::AddError An error has already been played recently, skipping this one";
    } else {
        if (sound_) sound_->stop();
        tErrorMessageTimer.start();
        AddMessage(ERROR_MESSAGE);
    }
}

void SoundMessages::AddMessage(QString message, bool queue) {
    if (queue) {
        messages_.enqueue(message);
        emit SIGNAL_MessageAdded();
    } else {
        QSound::play(message);
    }
}

}
