#ifndef ISUBSYSTEMUI_H
#define ISUBSYSTEMUI_H

#include <QObject>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::main {

class ISubSystemUI
{
public:
    virtual ~ISubSystemUI() { qDebug() << "~ISubSystemUI"; }

public slots:
    virtual void Configure(QString name) = 0;
    virtual void Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void Interrupt() = 0;

signals:
    virtual void SIGNAL_State_Off() = 0;
    virtual void SIGNAL_State_Intermediate() = 0;
    virtual void SIGNAL_State_Ready() = 0;
    virtual void SIGNAL_State_Unknown() = 0;
    virtual void SIGNAL_StartupFinished() = 0;
    virtual void SIGNAL_ShutdownFinished() = 0;
};

} // namespace

Q_DECLARE_INTERFACE(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI, "medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI")

#endif
