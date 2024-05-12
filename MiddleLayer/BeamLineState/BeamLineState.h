#ifndef MIDDLELAYER_BeamLineState_H
#define MIDDLELAYER_BeamLineState_H

#include <QSettings>
#include <QStateMachine>

#include "ior_interface.h"
#include "omron_interface.h"
#include "nsinglecontroller_interface.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::beamlinestate {


class BeamLineState : public QObject
{
    Q_OBJECT

public:
    explicit BeamLineState();
    ~BeamLineState();

public slots:

private slots:
    void SetupIOR();
    void SetupOmron();
    void SetupNSingles();

signals:
    void SIGNAL_CFState(QString name, bool extracted);
    void SIGNAL_QZState(QString name, bool extracted);
    void SIGNAL_DTState(bool extracted);
    void SIGNAL_GuillotineState(bool extracted);
    void SIGNAL_DiaphragmeState(bool extracted);
    void SIGNAL_DipoleState(QString name, bool on);

private:
    medicyc::cyclotron::OmronInterface omron_interface_;
    medicyc::cyclotron::IORInterface ior_interface_;
    medicyc::cyclotron::NSingleControllerInterface m1_interface_;
    medicyc::cyclotron::NSingleControllerInterface m2_interface_;
    medicyc::cyclotron::NSingleControllerInterface m4_interface_;
    const int OMRON_READINTERVAL = 500; // ms
};

}

#endif
