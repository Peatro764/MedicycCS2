#ifndef VIRTUALADC_H
#define VIRTUALADC_H

#include "IADC.h"
#include "Channel.h"

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::simulators::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

class VirtualADC : public QObject, public hw_adc::IADC
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::adc::IADC)
public:
    VirtualADC();

public slots:
    void SetConnected(bool yes);
    void Ping() override;
    void Read(QString channel_name) override;
    void SetChannels(const std::vector<hw_adc::Channel>& channels);
    bool ChannelExist(QString name) { return channels_.contains(name); }
    hw_adc::Channel GetChannel(QString name) { return channels_.value(name); }

private slots:

signals:
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_ReceivedChannelValue(QString channel, double physical_value, QString unit) override;
    void SIGNAL_ReadRequest(QString name);

private:
    QMap<QString, hw_adc::Channel> channels_;
    QMap<int, hw_adc::Channel> hashed_channels_;
    bool connected_ = false;
};

}

#endif // IADC_H
