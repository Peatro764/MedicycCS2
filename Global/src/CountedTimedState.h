#ifndef COUNTEDTIMEDSTATE_H
#define COUNTEDTIMEDSTATE_H

#include <QStateMachine>
#include <QState>
#include <QObject>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::global {

class CountedTimedState : public QState
{
    Q_OBJECT
public:
    CountedTimedState(QState *parent, int max_ncounts, QString count_message, int timeout_ms, QString timeout_message);

protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;

public slots:
    void Increment();
    void Reset();

private slots:

signals:
    void SIGNAL_ThresholdReached(QString count_message);
    void SIGNAL_Timeout(QString timeout_message);

private:
    int counter_ = 0;
    int max_ncounts_ = 0;
    QString count_message_;
    int timeout_ms_ = 0;
    QString timeout_message_;
    QTimer timeout_timer_;
};

}

#endif
