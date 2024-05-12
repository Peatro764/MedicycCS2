#include "VirtualNSingleContainer.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

VirtualNSingleContainer::VirtualNSingleContainer() {}

VirtualNSingleContainer::~VirtualNSingleContainer() {}

void VirtualNSingleContainer::Add(QString name, VirtualNSingle *v_nsingle) {
    v_nsingles_[name] = v_nsingle;
}

void VirtualNSingleContainer::Connected() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Connected();
    }
}

void VirtualNSingleContainer::Disconnected() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Disconnected();
    }
}

void VirtualNSingleContainer::IOLoad(double load) {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_IOLoad(load);
    }
}

void VirtualNSingleContainer::Local() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Local();
    }
}

void VirtualNSingleContainer::Remote() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Remote();
    }
}

void VirtualNSingleContainer::On() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_On();
    }
}

void VirtualNSingleContainer::On(QString rec) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_On();
    }
}

void VirtualNSingleContainer::Off() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Off();
    }
}

void VirtualNSingleContainer::Off(QString rec) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_Off();
    }
}

void VirtualNSingleContainer::Error() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Error();
    }
}

void VirtualNSingleContainer::Error(QString rec) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_Error();
    }
}

void VirtualNSingleContainer::Ok() {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_Ok();
    }
}

void VirtualNSingleContainer::Ok(QString rec) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_Ok();
    }
}

void VirtualNSingleContainer::Channel1Value(QString rec, double physical_value, bool polarity) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_Channel1Value(physical_value, polarity);
    }
}

void VirtualNSingleContainer::Channel1Value(QString rec, const Measurement& m) {
    if (v_nsingles_.contains(rec)) {
        Measurement m_virtual(v_nsingles_.value(rec)->config().GetChannel1ValueConfig());
        m_virtual.SetData(m.HexValue());
        emit v_nsingles_.value(rec)->SIGNAL_Channel1Value(m_virtual);
    }
}

void VirtualNSingleContainer::Channel1SetPoint(QString rec, double physical_value, bool polarity) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_Channel1SetPoint(physical_value, polarity);
    }
}

void VirtualNSingleContainer::Channel1SetPoint(QString rec, const Measurement& m) {
    if (v_nsingles_.contains(rec)) {
        Measurement m_virtual(v_nsingles_.value(rec)->config().GetChannel1SetPointConfig());
        m_virtual.SetData(m.HexValue());
        emit v_nsingles_.value(rec)->SIGNAL_Channel1SetPoint(m_virtual);
    }
}

void VirtualNSingleContainer::AddressValue(QString rec, const AddressRegister& address) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_AddressValue(address);
    }
}

void VirtualNSingleContainer::AddressSetPoint(QString rec, const AddressRegister& address) {
    if (v_nsingles_.contains(rec)) {
        emit v_nsingles_.value(rec)->SIGNAL_AddressSetPoint(address);
    }
}

void VirtualNSingleContainer::IOError(QString error) {
    for(auto name : v_nsingles_.keys()) {
        emit v_nsingles_.value(name)->SIGNAL_IOError(error);
    }
}

} // ns
