include(../MedicycCS2.pri)

SUBDIRS = NSingle \
    Electrometer \
    ClientConnection \
    IOR \
    Omron \
    ADC \
    RadiationMonitor \
    SignalGenerator \
    SourcePowerSupply \
    HFPowerSupplies

equals(RASPBERRYPI,Pi) {
  SUBDIRS += Raspberry
}

NSingle.depends = ClientConnection
#ADC.depends = ClientConnection
IOR.depends = ClientConnection
Omron.depends = ClientConnection
SourcePowerSupply.depends = ADC Omron
HFPowerSupply.depends = ADC Omron
#SignalGenerator.depends = ClientConnection
Electrometer.depends = ClientConnection
#RadiationMonitor.depends = ClientConnection

