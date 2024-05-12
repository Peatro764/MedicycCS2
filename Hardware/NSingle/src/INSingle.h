#ifndef INSINGLE_H
#define INSINGLE_H

#include <QObject>
#include <QDebug>

#include "Measurement.h"
#include "AddressRegister.h"
#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class INSingle
{
public:
    virtual ~INSingle() { qDebug() << "~INSingle"; }
    virtual NSingleConfig config() const = 0;
    virtual QString name() const = 0;

public slots:
    // exposed to dbus
    virtual void On() = 0;
    virtual void Off() = 0;
    virtual void Reset() = 0;

    // internal
    virtual void IsConnected() = 0;
    virtual void ReadState() = 0;
    virtual void ReadChannel1() = 0;
    virtual void Ping() = 0;
    virtual void ReadChannel1(int number) = 0;
    virtual void ReadChannel1SetPoint() = 0;
    virtual void WriteAndVerifyChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) = 0;
    virtual void WriteChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) = 0;
    virtual void ReadAddress() = 0;
    virtual void ReadAddressSetPoint() = 0;
    virtual void WriteAddressSetPoint(std::bitset<8> value) = 0;
    virtual void WriteAndVerifyAddressSetPoint(std::bitset<8> value) = 0;

signals:
    // dbus
    virtual void SIGNAL_Connected() = 0;
    virtual void SIGNAL_Disconnected() = 0;
    virtual void SIGNAL_On() = 0;
    virtual void SIGNAL_Off() = 0;
    virtual void SIGNAL_Error() = 0;
    virtual void SIGNAL_Ok() = 0;
    virtual void SIGNAL_Local() = 0;
    virtual void SIGNAL_Remote() = 0;
    virtual void SIGNAL_Channel1Value(double physical_value, bool polarity) = 0;
    virtual void SIGNAL_Channel1SetPoint(double physical_value, bool polarity) = 0;
    virtual void SIGNAL_IOLoad(double load) = 0;

    // internal
    virtual void SIGNAL_AddressValue(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) = 0;
    virtual void SIGNAL_AddressSetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) = 0;
    virtual void SIGNAL_Channel1Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) = 0;
    virtual void SIGNAL_Channel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m) = 0;
    virtual void SIGNAL_IOError(QString error) = 0;
    virtual void SIGNAL_ExecutionFinished() = 0;
    virtual void SIGNAL_ReplyReceived() = 0;
};

} // namespace

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::hardware::nsingle::INSingle, "medicyc::cyclotroncontrolsystem::hardware::nsingle::INSingle")

#endif
