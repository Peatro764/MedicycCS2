#ifndef COUNTEDSTATE_H
#define COUNTEDSTATE_H

#include <QStateMachine>
#include <QState>
#include <QObject>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::global {

class CountedState : public QState
{
    Q_OBJECT
public:
    CountedState(QState *parent, int max_ncounts, QString message);

protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;

public slots:
    void Increment();
    void Reset();

private slots:

signals:
    void SIGNAL_ThresholdReached(QString message);

private:
    int counter_ = 0;
    int max_ncounts_ = 0;
    QString message_;
};

}

#endif
