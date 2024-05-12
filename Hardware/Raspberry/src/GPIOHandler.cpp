#include "GPIOHandler.h"

#include <wiringPi.h>
#include <QSqlError>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::raspberry {

GPIOHandler::GPIOHandler() {
    this->start();

}

GPIOHandler::~GPIOHandler() {
    this->quit(); // stops thread
    while (this->isRunning()) {}
    if (worker_) delete worker_;
}

void GPIOHandler::run() {
  worker_ = new Worker;

  QObject::connect(worker_,  &Worker::SIGNAL_CW_Turn, this, &GPIOHandler::SIGNAL_DEC);
  QObject::connect(worker_,  &Worker::SIGNAL_CCW_Turn, this, &GPIOHandler::SIGNAL_INC);

  QObject::connect(this, &GPIOHandler::SIGNAL_INC, this, [&]() { qDebug() << "INC"; });
  QObject::connect(this, &GPIOHandler::SIGNAL_DEC, this, [&]() { qDebug() << "DEC"; });

  exec();  // start our own event loop
}

Worker::Worker(QObject* parent)
    : QObject(parent) {
    SetupGPIOPins();
    SetupTimers();
}

Worker::~Worker() {
}

void Worker::SetupGPIOPins() {
    wiringPiSetup();
    pinMode(CH_PB, INPUT);
    pinMode(CH_LED, OUTPUT);
    pinMode(CH_CLK_A, INPUT);
    pinMode(CH_CLK_B, INPUT);
}

void Worker::SetupTimers() {

    // PUSHBUTTON
    timer_read_pb_.setInterval(PB_READ_INTERVAL);
    timer_read_pb_.setSingleShot(false);
    timer_read_pb_.setTimerType(Qt::PreciseTimer);
    timer_read_pb_.start();
    QObject::connect(&timer_read_pb_, &QTimer::timeout, this, &Worker::Read_PB);

    // LED
    timer_input_allowed_.setInterval(INPUT_ALLOWED_INTERVAL);
    timer_input_allowed_.setSingleShot(true);
//    timer_input_allowed_.start(); // remove
    QObject::connect(&timer_input_allowed_, &QTimer::timeout, this, &Worker::TurnOffLED);

    // CLOCKS

//    timer_bounce_a_.setInterval(BOUNCE_WINDOW);
//    timer_bounce_b_.setInterval(BOUNCE_WINDOW);
//    timer_bounce_a_.setSingleShot(true);
//    timer_bounce_b_.setSingleShot(true);
//    timer_bounce_a_.setTimerType(Qt::PreciseTimer);
//    timer_bounce_b_.setTimerType(Qt::PreciseTimer);

//    QObject::connect(&timer_bounce_a_, &QTimer::timeout, this, [&]() {
////        qDebug() << QDateTime::currentMSecsSinceEpoch() << " Bounce CLK done " << clk_state_;
//        HandleCLK(state_clk_a_);
//    });

//    QObject::connect(&timer_bounce_b_, &QTimer::timeout, this, [&]() {
////        qDebug() << QDateTime::currentMSecsSinceEpoch() << " Bounce DT done " << dt_state_;
//        HandleDT(state_clk_b_);
//    });

    timer_cw_.setInterval(INTER_CHANNEL_FLIP_TIME);
    timer_ccw_.setInterval(INTER_CHANNEL_FLIP_TIME);
    timer_cw_.setSingleShot(true);
    timer_ccw_.setSingleShot(true);
    timer_cw_.setTimerType(Qt::PreciseTimer);
    timer_ccw_.setTimerType(Qt::PreciseTimer);

    timer_read_a_.setInterval(READ_INTERVAL);
    timer_read_b_.setInterval(READ_INTERVAL);
    timer_read_a_.setSingleShot(false);
    timer_read_b_.setSingleShot(false);
    timer_read_a_.start();
    timer_read_b_.start();
    timer_read_a_.setTimerType(Qt::PreciseTimer);
    timer_read_b_.setTimerType(Qt::PreciseTimer);

    QObject::connect(&timer_read_a_, &QTimer::timeout, this, &Worker::Read_CLK_A);
    QObject::connect(&timer_read_b_, &QTimer::timeout, this, &Worker::Read_CLK_B);
}

void Worker::Read_PB() {
    int state = digitalRead(CH_PB);
    if (state) TurnOnLED();
}

void Worker::TurnOnLED() {
    digitalWrite(CH_LED, 1);
    timer_input_allowed_.start();
}

void Worker::TurnOffLED() {
    digitalWrite(CH_LED, 0);
}

void Worker::Read_CLK_A() {
    int state = digitalRead(CH_CLK_A);
    if (state != state_clk_a_) {  qDebug() << QDateTime::currentMSecsSinceEpoch() << " CLK A flip " << state; Handle_CLK_A_Flank(state);}
 //   if (state != state_clk_a_) { qDebug() << QDateTime::currentMSecsSinceEpoch() << " Bounce CLK start " << state; timer_bounce_a_.start(); }
    state_clk_a_ = state;
}

void Worker::Read_CLK_B() {
    int state = digitalRead(CH_CLK_B);
    if (state != state_clk_b_) { qDebug() << QDateTime::currentMSecsSinceEpoch() << " CLK B flip " << state; Handle_CLK_B_Flank(state);}
//    if (state != state_clk_b_) { timer_bounce_b_.start(); /*qDebug() << QDateTime::currentMSecsSinceEpoch() << " Bounce DT start " << state; */}
    state_clk_b_ = state;
}

void Worker::Handle_CLK_A_Flank(bool state) {
    if (state) {
        if (timer_ccw_.isActive()) {
            TurnDetected(CCW);
            timer_ccw_.stop();
        } else if (!timer_cw_.isActive()) {
            timer_cw_.start();
        }
    }
}

void Worker::Handle_CLK_B_Flank(bool state) {
    if (!state) {
        if (timer_cw_.isActive()) {
            TurnDetected(CW);
            timer_cw_.stop();
        } else if (!timer_ccw_.isActive()) {
            timer_ccw_.start();
        }
    }
}

void Worker::TurnDetected(int direction) {
    if (timer_input_allowed_.isActive()) {
        timer_input_allowed_.start(); // prolong duration
        if (direction == CW) {
            emit SIGNAL_CW_Turn();
        } else {
            emit SIGNAL_CCW_Turn();
        }
    } else {
        qDebug() << "Ignoring turn since input is not currently allowed";
    }
}

}
