#ifndef VIRTUALIOR_H
#define VIRTUALIOR_H

#include "IIOR.h"
#include "Channel.h"

#include <QObject>
#include <QDebug>

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

namespace medicyc::cyclotroncontrolsystem::simulators::ior {

class VirtualIOR : public QObject, public hw_ior::IIOR
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::ior::IIOR)
public:
    VirtualIOR();

public slots:
    void SetChannels(const std::vector<hw_ior::Channel>& channels);
    bool ChannelExist(QString name) { return channels_.contains(name); }
    hw_ior::Channel GetChannel(QString name) { return channels_.value(name); }

    void ReadChannelValue(QString name) override;
    void ReadChannelMask(QString name) override;

private slots:

signals:
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_ReceivedChannelMask(QString channel, bool mask) override;
    void SIGNAL_ReceivedChannelValue(QString channel, bool value) override;

    void SIGNAL_ReadValueRequest(QString name);
    void SIGNAL_ReadBlockRequest(QString name);

private:
    QMap<QString, hw_ior::Channel> channels_;
    QMap<int, hw_ior::Channel> hashed_channels_;
};

}

#endif
