#ifndef VIRTUAL_NSINGLE_CONTAINER_H
#define VIRTUAL_NSINGLE_CONTAINER_H

#include "VirtualNSingle.h"

#include <QObject>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class VirtualNSingleContainer : public QObject
{
    Q_OBJECT
public:
    VirtualNSingleContainer();
    ~VirtualNSingleContainer();
    void Add(QString name, VirtualNSingle *v_nsingle);

public slots:
    void Connected();
    void Disconnected();
    void IOLoad(double load);
    void Local();
    void Remote();
    void On();
    void Off();
    void Error();
    void Ok();
    void On(QString rec);
    void Off(QString rec);
    void Error(QString rec);
    void Ok(QString rec);
    void Channel1Value(QString rec, double physical_value, bool polarity);
    void Channel1Value(QString rec, const nsingle::Measurement& m);
    void Channel1SetPoint(QString rec, double physical_value, bool polarity);
    void Channel1SetPoint(QString rec, const nsingle::Measurement& m);
    void AddressValue(QString rec, const nsingle::AddressRegister& address);
    void AddressSetPoint(QString rec, const nsingle::AddressRegister& address);

    void IOError(QString error);

private:
    QMap<QString, VirtualNSingle*> v_nsingles_;
};

} // ns

#endif
