#ifndef IElectrometer_H
#define IElectrometer_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::electrometer {

class IElectrometer
{
public:
    virtual ~IElectrometer() { qDebug() << "~IElectrometer"; }

public slots:
//    virtual void Read(QString channel_name) = 0;

signals:
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0;

private:
};

}

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::electrometer::IElectrometer, "medicyc::cyclotroncontrolsystem::hardware::electrometer::IElectrometer")

#endif
