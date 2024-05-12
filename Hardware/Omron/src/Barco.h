#ifndef BARCO_H
#define BARCO_H

#include "Omron_global.h"

#include <QObject>
#include <QString>
#include <QSettings>
#include <QStateMachine>
#include <QDateTime>
#include <memory>
#include <queue>
#include <QTimer>
#include <QMap>

#include "SocketClient.h"
#include "FINS.h"
#include "MemoryAreaCommand.h"
#include "BarcoMessage.h"

#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class OMRON_EXPORT Barco : public QObject
{
    Q_OBJECT
public:
    Barco();
    ~Barco();
    bool IsConnected() const;
    void SetMessages(QMap<int, medicyc::cyclotroncontrolsystem::hardware::omron::BarcoMessage> messages);

public slots:

private slots:
    void RequestNodeAddress();
    void ProcessIncomingData(QByteArray data);
    void ProcessNodeAddressReply(QByteArray data);
    void ProcessFrameSendReply(QByteArray data);

    void ErrorMessage(QString message);
    void WarningMessage(QString message);
    void InfoMessage(QString message);
    void DebugMessage(QString message);

signals:
    void SIGNAL_Connected();
    void SIGNAL_Disconnected();
    void SIGNAL_NodeAddressReceived(medicyc::cyclotroncontrolsystem::hardware::omron::FINSHeader header);
    void SIGNAL_IOError(QString message);

private:
    void SetupStateMachine();
    void StateMachineMessage(QString message);
    void ProcessBarcoMessage(int index);

    QStateMachine sm_;
    std::unique_ptr<QSettings> settings_;
    SocketClient socket_client_;
    FINSHeader fins_header_;
    int reply_timeout_ = 0; // ms
    int ping_interval_ = 1000; // ms
    const size_t MAXQUEUESIZE = 50;

    const QString SYSTEM = "Barco";

    QMap<int, omron::BarcoMessage> messages_;
    medicyc::cyclotron::MessageLoggerInterface logger_;
};

}

#endif

