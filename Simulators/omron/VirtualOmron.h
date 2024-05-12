#ifndef VIRTUALOMRON_H
#define VIRTUALOMRON_H

#include "IOmron.h"

#include <QObject>
#include <QString>
#include <QSettings>
#include <QDateTime>
#include <memory>
#include <queue>
#include <QTimer>
#include <QMap>

#include "Channel.h"

namespace hw_omron = medicyc::cyclotroncontrolsystem::hardware::omron;

namespace medicyc::cyclotroncontrolsystem::simulators::omron {

class VirtualOmron : public QObject, public hw_omron::IOmron
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::hardware::omron::IOmron)
public:
    VirtualOmron();
    ~VirtualOmron();

    void SetChannels(const QMap<QString, hw_omron::Channel>& channels);

public slots:
    void SetConnected(bool yes);
    void Ping() override;
    void ReadChannel(QString name) override;
    void WriteChannel(QString name, bool content) override;

    hw_omron::Channel GetChannel(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const;
    hw_omron::Channel GetChannel(QString name) const;
    bool ChannelExist(uint8_t node, hw_omron::FINS_MEMORY_AREA memory_area, uint16_t address, uint8_t bit) const;
    bool ChannelExist(QString name) const;

private slots:

signals:
    void SIGNAL_Connected() override;
    void SIGNAL_Disconnected() override;
    void SIGNAL_IOError(QString error) override;
    void SIGNAL_BitWritten(QString channel, bool content) override;
    void SIGNAL_BitRead(QString channel, bool content) override;

    void SIGNAL_ReadRequest(QString name);
    void SIGNAL_WriteRequest(QString name, bool content);

private: 
    QMap<QString, hw_omron::Channel> channels_;
    QMap<QString, hw_omron::Channel> hashed_channels_;
    bool connected_ = false;
};

}

#endif // OMRON_H
