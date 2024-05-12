#ifndef NSINGLE_H
#define NSINGLE_H

#include <QObject>
#include <QString>

#include "SocketClient.h"

class NSingle : public QObject
{
    Q_OBJECT
public:
    NSingle(QString ip, int port);
    void Configure();

public slots:
    void Off();
    void StandBy();
    void On();
    void Reset();

    void WriteRegisterE(qint8 ref);
    void WriteCurrentReference(qint16 ref);
    void WriteCurrentTolerance(qint16 tol, bool enable = true, bool filtre = false);
    void WriteConfig(QString hex);

    void ReadState();
    void ReadConfig();
    void ReadRegisterF();
    void ReadCurrentReference();
    void ReadCurrentMeasurement();

private slots:
    void ProcessIncomingData(QString data);

signals:
    void Connected();
    void Disconnected();
    void CommandError(QString msg);
    void DeviceError(QString msg);

    // Status parameters
    void Off(bool state);
    void StandBy(bool state);
    void On(bool state);
    void Ok(bool state);
    void Local(bool state);
    void Distance(bool state);
    void Switch1(bool state);
    void Switch2(bool state);

    //Configuration parameters
    void Type(int type);
    void Polarity(int type);
    void Filtering(int type);
    void MeasurementType(int type);
    void StatusMonitoring(int type);

    void CurrentMeasurement(qint16 value);
    void CurrentReference(qint16 value);

private:
    void ProcessStateChange(QString body);
    void ProcessError(QString body);
    void ProcessConfig(QString body);
    void ProcessCurrentMeasurement(QString body);
    void ProcessCurrentReference(QString body);

    SocketClient socket_client_;

    const QString cmd_footer_ = "*\r\n";
};

#endif // NSINGLE_H
