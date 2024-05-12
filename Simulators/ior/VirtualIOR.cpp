#include "VirtualIOR.h"
#include "IORUtil.h"

namespace hw_ior = medicyc::cyclotroncontrolsystem::hardware::ior;

namespace medicyc::cyclotroncontrolsystem::simulators::ior {

VirtualIOR::VirtualIOR() {

}

void VirtualIOR::SetChannels(const std::vector<hw_ior::Channel>& channels) {
    for  (hw_ior::Channel ch : channels) {
        channels_[ch.Name()] = ch;
        hashed_channels_[hw_ior::ior_util::Hash(ch.Card(), ch.Block(), ch.Address())] = ch;
    }
}

void VirtualIOR::ReadChannelValue(QString name) {
    emit SIGNAL_ReadValueRequest(name);
}

void VirtualIOR::ReadChannelMask(QString name) {
    emit SIGNAL_ReadBlockRequest(name);
}


}
