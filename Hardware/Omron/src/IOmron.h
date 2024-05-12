#ifndef IOMRON_H
#define IOMRON_H

#include <QObject>
#include <QDebug>


namespace medicyc::cyclotroncontrolsystem::hardware::omron {

class IOmron
{
public:
    virtual ~IOmron() { qDebug() << "~IOmron"; }

public slots:
    virtual void Ping() = 0;
    virtual void ReadChannel(QString name) = 0;
    virtual void WriteChannel(QString name, bool content) = 0;

private slots:

signals:
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0 ;
    virtual void SIGNAL_IOError(QString error) = 0;
    virtual void SIGNAL_BitWritten(QString channel, bool content) = 0;
    virtual void SIGNAL_BitRead(QString channel, bool content) = 0;

private:

};

}

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::omron::IOmron, "medicyc::cyclotroncontrolsystem::hardware::omron::IOmron")

#endif // IOMRON_H
