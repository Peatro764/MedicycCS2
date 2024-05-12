#ifndef IIOR_H
#define IIOR_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class IIOR
{
public:
    virtual ~IIOR() { qDebug() << "~IIOR"; }

public slots:
    virtual void ReadChannelValue(QString name) = 0;
    virtual void ReadChannelMask(QString name) = 0;

signals:
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0;
    virtual void SIGNAL_IOError(QString error) = 0;
    virtual void SIGNAL_ReceivedChannelValue(QString channel, bool value) = 0;
    virtual void SIGNAL_ReceivedChannelMask(QString channel, bool mask) = 0;

private:
};

}

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::ior::IIOR, "medicyc::cyclotroncontrolsystem::hardware::ior::IIOR")

#endif
