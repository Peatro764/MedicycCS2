#ifndef RASPBERRY_GPIOHANDLER_H
#define RASPBERRY_GPIOHANDLER_H

#include <QString>
#include <QDateTime>
#include <QThread>
#include <memory>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::hardware::raspberry {

class Worker : public QObject {
  Q_OBJECT

   public:
    Worker(QObject* parent = 0);
    ~Worker();

public:

private slots:
    void SetupGPIOPins();
    void SetupTimers();
    void Read_PB();
    void TurnOnLED();
    void TurnOffLED();
    void Read_CLK_A();
    void Read_CLK_B();
    void Handle_CLK_A_Flank(bool state);
    void Handle_CLK_B_Flank(bool state);
    void TurnDetected(int direction);

signals:
    void SIGNAL_CLK_A_Flank(bool state);
    void SIGNAL_CLK_B_Flank(bool state);
    void SIGNAL_CW_Turn();
    void SIGNAL_CCW_Turn();

private:
    // Pushbuttons and lights
    const int CH_PB = 3; // Physical 15
    const int CH_LED = 4; // Physical 16
    const int PB_READ_INTERVAL = 100;
    const int INPUT_ALLOWED_INTERVAL = 30000;
    QTimer timer_read_pb_;
    QTimer timer_input_allowed_;

    // CLOCKS
    const int CW = 1; // Physical 11
    const int CCW = 0; // Physical 13
    const int CH_CLK_A = 0;
    const int CH_CLK_B = 2;
//    const int BOUNCE_WINDOW = 10;
    int READ_INTERVAL = 4;
    int INTER_CHANNEL_FLIP_TIME = 300;//50; // max allowed time from channel X flank -> channel Y flank
    QTimer timer_read_a_;
    QTimer timer_read_b_;
    QTimer timer_cw_;
    QTimer timer_ccw_;
    int state_clk_a_ = 0;
    int state_clk_b_ = 0;
//    QTimer timer_bounce_a_;
//    QTimer timer_bounce_b_;
};

class GPIOHandler : public QThread {
    Q_OBJECT
public:
    GPIOHandler();
     ~GPIOHandler();

private slots:

signals: // public
    void SIGNAL_INC();
    void SIGNAL_DEC();

signals: // private

private:
    void run() override;
    Worker *worker_ = nullptr;
};

}

#endif
