#ifndef ISIGNALGENERATOR_H
#define ISIGNALGENERATOR_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::signalgenerator {

class ISignalGenerator
{
public:
    virtual ~ISignalGenerator() { qDebug() << "~ISignalGenerator"; }

public slots:
//    virtual void Read(QString channel_name) = 0;

signals:
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0;

private:
};

}

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::signalgenerator::ISignalGenerator, "medicyc::cyclotroncontrolsystem::hardware::signalgenerator::ISignalGenerator")

#endif
