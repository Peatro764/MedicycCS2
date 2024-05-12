#ifndef NSINGLE_RegulatedParameter_H
#define NSINGLE_RegulatedParameter_H

#include <QObject>

#include "Measurement.h"
#include "NSingleConfig.h"

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class RegulatedParameter : public QObject
{
    Q_OBJECT

public:
    RegulatedParameter(NSingleConfig config);
    ~RegulatedParameter();

    // SetPoint = The value sent to the power supply to achieve ActValue=DesValue
    // DesValue = The desired value of the power supply
    // ActValue = The actual value of the power supply
    Measurement GetSetPoint() const;
    bool        GetActPolarity() const;
    Measurement GetActValue() const;
    Measurement GetDesValue() const;

    // Mostly for unit testing, otherwise these could be private
    double Mean() const { return mean_; } // in bit register unit
    double StdDev() const { return stddev_; } // in bit register unit
    double StdErr() const { return stderr_; } // in bit register unit
    bool NearTarget() const;
    bool NearZero() const;
    bool SignalSteady() const;
    bool PolarityCorrect() const;
    bool OnTarget() const;

public slots:
    void ActValueReceived(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    void SetPointReceived(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);

    // Set a physical value and polarity
    void SetDesValue(const medicyc::cyclotroncontrolsystem::hardware::nsingle::Measurement& m);
    // Set a physical value but dont change polarity. Only value >= 0 allowed.
    void SetDesPhysicalValue(double value);
    // Set a physical value and polarity. Only value >=0 allowed.
    void SetDesPhysicalValue(double value, bool polarity);
    void SetZeroDesValueWithCurrentPolarity();
    void SetCurrentPolarityInvertedZeroDesValue();
    void ChangePolarity();
    void SetActAsDesValue();
    // Incs or decs des value and trigger a regulation loop
    void IncrementDesValue(int nbits);
    void DecrementDesValue(int nbits);
    // Incs or decs both set and des value and dont trigger a regulation loop
    void IncrementSetAndDesValue(int nbits);
    void DecrementSetAndDesValue(int nbits);
    int DesAndActValueDifference(); // in bit register unit

    void SetDefaultTolerance();
    void SetDoubleTolerance();
    void SetReadyTolerance();

    void SetPointToDesPoint();
    void SetImprovedSetPoint();

    void SetRampParameters();
    void IncRampSetPoint();
    void SetRampSetPoint();

signals:
    void SIGNAL_SignalChange();
    void SIGNAL_SignalSteady();
    void SIGNAL_NearTarget();
    void SIGNAL_NearZero();
    void SIGNAL_OnTarget();
    void SIGNAL_OffTarget();
    void SIGNAL_PolarityCorrect();
    void SIGNAL_PolarityWrong();
    void SIGNAL_SetPointsMatch();
    void SIGNAL_SetPointsDontMatch();
    // physical_value always >= 0
    void SIGNAL_DesValue(double physical_value, bool polarity);
    void SIGNAL_DesUpdated(double physical_value, bool polarity);
    void SIGNAL_ActUpdated(double physical_value, bool polarity);
    void SIGNAL_DesValueOutOfBounds();

    void SIGNAL_RampFinished();

private:
    void CalculateSignalParameters();
    void CheckSignalParameters();

    NSingleConfig config_;

    std::vector<Measurement> act_values_; // value read from the power supply
    Measurement des_value_; // value we are trying to have on the power supply
    Measurement set_value_; // command value given to the power supply to achieve (act_value_= des_value_)
    int buffer_size_;
    double mean_ = 0.0;
    double stddev_ = 0.0;
    double stderr_ = 0.0;
    double mean_tol_ = 0.0;
    double std_tol_ = 0.0;
    struct {
        double start = 1;
        double stop = 1;
        double current = 1;
        int dir = 1;
        double delta = 1.0; // physical increment (Ampere or Volt)
    } ramp_params_;
};

} // namespace

#endif
