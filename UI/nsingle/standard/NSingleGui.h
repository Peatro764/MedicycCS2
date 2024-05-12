#ifndef NSINGLEGUI_H
#define NSINGLEGUI_H

#include <QWidget>
#include <memory>
#include <QSettings>
#include <QVBoxLayout>
#include <QMap>

#include "NSingle.h"
#include "NSingleRepo.h"
#include "NSingleWidget.h"

namespace Ui {
class NSingleGui;
}

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

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
    void DisplayChannel1Value(const hw_nsingle::Measurement& m);
    void DisplayChannel1SetPoint(const hw_nsingle::Measurement& m);
    void UpdateConfig(hw_nsingle::Config config);
    void UpdateError(hw_nsingle::Error error);
    void UpdateState(hw_nsingle::StateReply state);
    void WriteConfig();

private slots:
    void Channel1SetPointValueUpdated();
    void Channel1SetPointBitSetUpdated(int value);

    void Channel1ToleranceValueUpdated();
    void Channel1ToleranceBitSetUpdated(int value);

    void AddressSetPointValueUpdated(int value);
    void AddressSetPointBitSetUpdated(int value);


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
    void ConnectNSingleSignals(QString name);
    void DeconnectNSingleSignals(QString name);
    void ReadNSingleState();
    void CreateNSingles();
    bool Ch1SetPointSign() const;
    bool Ch1ValueSign() const;
    QString SignToString(bool sign) const;
    Ui::NSingleGui *ui_;
    std::unique_ptr<medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleRepo> repo_;

    QGridLayout *gridLayout_nsingles_;
    QMap<QString, NSingleWidget*> widgets_;
    QMap<QString, hw_nsingle::NSingle*> nsingles_;
    QString selected_nsingle_ = "";
};

} // ns

#endif
