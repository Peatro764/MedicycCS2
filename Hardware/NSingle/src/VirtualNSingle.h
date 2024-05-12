#ifndef VIRTUAL_NSINGLE_H
#define VIRTUAL_NSINGLE_H

#include "INSingle.h"
#include "NSingle_global.h"

#include <QObject>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT VirtualNSingle : public QObject, public INSingle
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::nsingle::INSingle)

public:
    VirtualNSingle(NSingleConfig config);
    ~VirtualNSingle();

    QString name() const override { return config_.name(); }
    NSingleConfig config() const override { return config_; }

public slots:
    // from dbus or controller (dbus -> virtualnsingle -> multiplexnsingle, controller -> virtualnsingle -> multiplexnsingle)
    void On() override { emit SIGNAL_CommandOn(name()); }
    void Off() override { emit SIGNAL_CommandOff(name()); }
    void Reset() override { emit SIGNAL_CommandReset(name()); }

    // from controller (controller -> virtualnsingle -> multiplexnsingle)
    void IsConnected() override { emit SIGNAL_CheckConnection(); }
    void ReadState() override { emit SIGNAL_CommandReadState(name()); }
    void ReadChannel1() override { emit SIGNAL_CommandReadChannel1(name()); }
    void Ping() override { emit SIGNAL_CommandPing(name()); }
    void ReadChannel1(int number) override { emit SIGNAL_CommandReadChannel1(name(), number); }
    void WriteAndVerifyChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m) override { emit SIGNAL_CommandWriteAndVerifyChannel1SetPoint(name(), m); }
    void WriteChannel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m) override { emit SIGNAL_CommandWriteChannel1SetPoint(name(), m); }
    void ReadChannel1SetPoint() override { emit SIGNAL_CommandReadChannel1SetPoint(name()); }
    void ReadAddress() override { emit SIGNAL_CommandReadAddress(name()); }
    void ReadAddressSetPoint() override { emit SIGNAL_CommandReadAddressSetPoint(name()); }
    void WriteAddressSetPoint(std::bitset<8> address) override { emit SIGNAL_CommandWriteAddressSetPoint(name(), address); }
    void WriteAndVerifyAddressSetPoint(std::bitset<8> address) override { emit SIGNAL_CommandWriteAndVerifyAddressSetPoint(name(), address); }

private slots:

signals:
    // to dbus or controller (from multiplexed)
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_On() override;
    void SIGNAL_Off() override;
    void SIGNAL_Error() override;
    void SIGNAL_Ok() override;
    void SIGNAL_Local() override;
    void SIGNAL_Remote() override;
    void SIGNAL_Channel1Value(double physical_value, bool polarity) override;
    void SIGNAL_Channel1SetPoint(double physical_value, bool polarity) override;
    void SIGNAL_IOLoad(double load) override;

    //to multiplexnsingle (from dbus or controller)
    void SIGNAL_CommandOn(QString name);
    void SIGNAL_CommandOff(QString name);
    void SIGNAL_CommandReset(QString name);

    // to multiplexnsingle (from controller)
    void SIGNAL_CommandReadState(QString name);
    void SIGNAL_CommandPing(QString name);
    void SIGNAL_CommandReadChannel1(QString name);
    void SIGNAL_CommandReadChannel1(QString name, int number);
    void SIGNAL_CommandWriteAndVerifyChannel1SetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m);
    void SIGNAL_CommandWriteChannel1SetPoint(QString name, const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m);
    void SIGNAL_CommandReadChannel1SetPoint(QString name);
    void SIGNAL_CommandReadAddress(QString name);
    void SIGNAL_CommandReadAddressSetPoint(QString name);
    void SIGNAL_CommandWriteAddressSetPoint(QString name, std::bitset<8> address);
    void SIGNAL_CommandWriteAndVerifyAddressSetPoint(QString name, std::bitset<8> address);
    void SIGNAL_CheckConnection();

    // to controller (from multiplexnsingle)
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_Channel1Value(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m) override;
    void SIGNAL_Channel1SetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement &m) override;
    void SIGNAL_AddressValue(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) override;
    void SIGNAL_AddressSetPoint(const medicyc::cyclotroncontrolsystem::hardware::nsingle::AddressRegister& address) override;

    void SIGNAL_ExecutionFinished() override;
    void SIGNAL_ReplyReceived() override;

private:
    NSingleConfig config_;
};

} // namespace

#endif
