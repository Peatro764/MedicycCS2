#include "VideoWorker.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QThread>
#include <QDateTime>

#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

namespace global = medicyc::cyclotroncontrolsystem::global;

VideoWorker::VideoWorker(QObject* parent)
    : QObject(parent) {
    qDebug() << "VideoWorker::VideoWorker";
    qRegisterMetaType<cv::Mat>("cv::Mat");
}

VideoWorker::~VideoWorker() {
    if (video.isOpened()) {
        video.release();
    }
}

void VideoWorker::SetupStream() {
    qDebug() << "VideoWorker::SetupStream " << QThread::currentThreadId();
    QStateMachine *sm = new QStateMachine(this);

    // States
    QState *sSuperState = new QState();
        global::TimedState *sWaitingForFeed = new global::TimedState(sSuperState, 2000, "Reentry");
        QState *sReadingFeed = new QState(sSuperState);
        global::TimedState *sDelay = new global::TimedState(sSuperState, read_delay_, "Delay", sReadingFeed);

    // SuperState
    sSuperState->setInitialState(sWaitingForFeed);

    // WaitingForFeed
    QObject::connect(sWaitingForFeed, &QState::entered, this, [&](){ emit SIGNAL_NoVideoFeed(camera_channel_); });
    QObject::connect(sWaitingForFeed, &QState::entered, this, &VideoWorker::OpenVideoFeed);
    sWaitingForFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedOpen, sReadingFeed);

    // ReadingFeed
    QObject::connect(sReadingFeed, &QState::entered, this, [&]() { qApp->processEvents(); });
    QObject::connect(sReadingFeed, &QState::entered, this, &VideoWorker::ReadVideoFeed);
    sReadingFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedRead, sDelay);
    sReadingFeed->addTransition(this, &VideoWorker::SIGNAL_VideoFeedClosed, sWaitingForFeed);

    // Delay
    (void)sDelay;

    sm->addState(sSuperState);
    sm->setInitialState(sSuperState);
    sm->start();
}

void VideoWorker::OpenVideoFeed() {
    qDebug() << "VideoWorker::OpenVideoFeed";
    if (video.open(camera_channel_)) {
        emit SIGNAL_VideoFeedOpen();
    } else {
        emit SIGNAL_VideoFeedClosed();
    }
}

void VideoWorker::CloseVideoFeed() {
    qDebug() << "VideoWorker::CloseVideoFeed";
    video.release();
    emit SIGNAL_VideoFeedClosed();
}

void VideoWorker::ReadVideoFeed() {
//    qDebug() << "VideoWorker::ReadVideoFeed";
    if(video.isOpened()) {
        cv::Mat frame;
        video >> frame;
        emit SIGNAL_VideoFeedRead(frame);
    } else {
        emit SIGNAL_VideoFeedClosed();
    }
}

void VideoWorker::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "MEM " << QDateTime::currentDateTime() << " ->" << name << " " << QThread::currentThreadId(); });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "MEM " << QDateTime::currentDateTime() << " <-" << name << " " << QThread::currentThreadId(); });
}

}
