#ifndef MONITOREDSTATE_H
#define MONITOREDSTATE_H

#include <QStateMachine>
#include <QState>
#include <QObject>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::global {

class MonitoredState : public QState
{
    Q_OBJECT
public:
    MonitoredState(QState *parent, int timeout_ms, QString message, int max_tries, QAbstractState *error_state);

protected:
    void onEntry(QEvent *event) override;
    void onExit(QEvent *event) override;

public slots:

private slots:
    void IncrementCounter();

signals:
    void SIGNAL_DoWork();
    void SIGNAL_Timeout(QString message);
    void SIGNAL_Bailout(QString message);

private:
    QTimer timeout_timer_;
    int n_tries_ = 0;
    int max_tries_;
};

}

#endif
