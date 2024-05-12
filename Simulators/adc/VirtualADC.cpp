#include "VirtualADC.h"
#include "ADCUtil.h"

namespace medicyc::cyclotroncontrolsystem::simulators::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

VirtualADC::VirtualADC() {

}

void VirtualADC::SetChannels(const std::vector<hw_adc::Channel>& channels) {
    for  (const hw_adc::Channel& ch : channels) {
        channels_[ch.Name()] = ch;
        hashed_channels_[hw_adc::util::Hash(ch.Card(), ch.Address())] = ch;
    }
}

void VirtualADC::Read(QString channel_name) {
    emit SIGNAL_ReadRequest(channel_name);
}

void VirtualADC::Ping() {
    if (connected_)
        emit SIGNAL_Connected();
    else
        emit SIGNAL_Disconnected();
}

void VirtualADC::SetConnected(bool yes) {
    connected_ = yes;
    if (connected_)
        emit SIGNAL_Connected();
    else
        emit SIGNAL_Disconnected();
}


}
