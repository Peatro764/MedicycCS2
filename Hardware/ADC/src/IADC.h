#ifndef IADC_H
#define IADC_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::adc {

class IADC
{
public:
    virtual ~IADC() { qDebug() << "~IADC"; }

public slots:
    virtual void Ping() = 0;
    virtual void Read(QString channel_name) = 0;

signals:
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0;
    virtual void SIGNAL_IOError(QString error) = 0;
    virtual void SIGNAL_ReceivedChannelValue(QString channel, double physical_value, QString unit) = 0;

private:
};

}

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::adc::IADC, "medicyc::cyclotroncontrolsystem::hardware::adc::IADC")

#endif // IADC
