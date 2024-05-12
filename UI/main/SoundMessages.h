#ifndef SOUNDMESSAGES_H
#define SOUNDMESSAGES_H

#include <QObject>
#include <QQueue>
#include <QStateMachine>
#include <QSound>
#include <QTimer>

namespace medicyc::cyclotroncontrolsystem::ui::main {

class SoundMessages : public QObject
{
    Q_OBJECT
public:
    explicit SoundMessages(QObject *parent = nullptr);
    void AddMessage(QString name, bool queue=true);
    void AddErrorMessage();

signals:
    void SIGNAL_MessageFinished();
    void SIGNAL_MessageAdded();

private slots:
    void CheckQueue();
    void PopQueue();
    void CheckMessageStatus();

private:
    void SetupStateMachine();
    QQueue<QString> messages_;
    QStateMachine sm_;
    QSound* sound_ = nullptr;
    QTimer tErrorMessageTimer;
    const QString ERROR_MESSAGE = QString(":/sounds/Error.wav");
};

}

#endif // SOUNDMESSAGES_H
