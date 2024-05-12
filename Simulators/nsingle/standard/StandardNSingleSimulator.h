#ifndef STANDARDNSINGLESIMULATOR_H
#define STANDARDNSINGLESIMULATOR_H

#include <QWidget>
#include <QTimer>
#include <QRandomGenerator>

#include "VirtualNSingle.h"
#include "AddressRegister.h"
#include "Error.h"

namespace Ui {
class StandardNSingleSimulator;
}

namespace medicyc::cyclotroncontrolsystem::simulators::nsingle::standard {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class StandardNSingleSimulator : public QWidget
{
    Q_OBJECT

public:
    explicit StandardNSingleSimulator(hw_nsingle::VirtualNSingle *nsingle, QWidget *parent = 0);
    ~StandardNSingleSimulator();

public slots:
    void DisplayAddressSetPoint(const hw_nsingle::AddressRegister& address);
    void DisplayAddressValue(const hw_nsingle::AddressRegister& address);
    void DisplayChannel1Value(const hw_nsingle::Measurement& m);
    void DisplayChannel1SetPoint(const hw_nsingle::Measurement& m);
    void DisplayChannel2Value(const hw_nsingle::Measurement& m);

private slots:
    void IncrementChannel1Value();
    void DecrementChannel1Value();
    void Channel1ValueUpdated();
    void Channel1SetUpdated();
    void Channel1ValueBitSetUpdated();

    void AddressValueUpdated();
    void AddressValueBitSetUpdated();

private:
    void ConnectGuiSignals();
    bool Ch1SetPointSign() const;
    bool Ch1ValueSign() const;
    bool Ch2ValueSign() const;
    QString SignToString(bool sign) const;
    Ui::StandardNSingleSimulator *ui_;
    hw_nsingle::VirtualNSingle *nsingle_;
    QTimer ping_timer_;
    QRandomGenerator rand_;
    bool connected_ = true;
};

} // ns

#endif
