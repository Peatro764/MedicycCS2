#ifndef NSINGLEGUI_H
#define NSINGLEGUI_H

#include <QWidget>
#include <memory>
#include <QSettings>
#include <QVBoxLayout>
#include <QMap>

#include "MultiplexedNSingle.h"
#include "NSingle.h"
#include "NSingleRepo.h"
#include "NSingleWidget.h"

namespace Ui {
class NSingleGui;
}

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::multiplexed {

namespace hw_nsingle = medicyc::cyclotroncontrolsystem::hardware::nsingle;

class NSingleGui : public QWidget
{
    Q_OBJECT

public:
    explicit NSingleGui(QWidget *parent = 0);
    ~NSingleGui();

public slots:
    void WriteAddressSetPoint();
    void WriteChannel1SetPoint();
    void WriteChannel1Tolerance();

    void DisplayAddressSetPoint(const hw_nsingle::AddressRegister& address);
    void DisplayAddressValue(const hw_nsingle::AddressRegister& address);
    void DisplayChannel1Value(QString name, const hw_nsingle::Measurement& m);
    void DisplayChannel1SetPoint(QString name, const hw_nsingle::Measurement& m);
    void UpdateConfig(QString name, hw_nsingle::Config config);
    void UpdateError(QString name, hw_nsingle::Error error);
    void UpdateState(QString name, hw_nsingle::StateReply state);
    void WriteConfig();

private slots:
    void Channel1SetPointValueUpdated();
    void Channel1SetPointBitSetUpdated();

    void Channel1ToleranceValueUpdated();
    void Channel1ToleranceBitSetUpdated();

    void AddressSetPointValueUpdated();
    void AddressSetPointBitSetUpdated();


    void ChangeState(hw_nsingle::StateCommandType cmd);
    void ReadAddress();
    void ReadAddressSetPoint();
    void ReadChannel1();
    void ReadChannel1SetPoint();
    void ReadError();
    void ReadConfig();
    void ReadState();

    void SelectNSingle(QString name);
//    void ToleranceValueUpdated();
//    void ToleranceBitSetUpdated();

//    void AddressSetPointValueUpdated();
//    void AddressSetPointBitSetUpdated();

private:
    void SetupLayout();
    void ConnectGuiSignals();
    void SetupDatabase();
    void FillComboBoxes();
    void FillAvailableNSingles();
    void ConfigureNSingle(QString name);
    void ConnectNSingleSignals();
    void ReadNSingleState();
    void CreateNSingles();
    bool Ch1SetPointSign() const;
    bool Ch1ValueSign() const;
    bool Ch2ValueSign() const;
    QString SignToString(bool sign) const;
    Ui::NSingleGui *ui_;
    std::unique_ptr<medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo> repo_;

    QGridLayout *gridLayout_nsingles_;
    QMap<QString, NSingleWidget*> widgets_;
    QString selected_nsingle_ = "";
    hw_nsingle::MultiplexedNSingle *nsingle_;
};

} // ns

#endif
