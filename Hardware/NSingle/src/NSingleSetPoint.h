#ifndef HARDWARE_NSINGLESETPOINT_H
#define HARDWARE_NSINGLESETPOINT_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSingleSetPoint {
public:
    NSingleSetPoint(bool powered_up, bool sign, double value);
    NSingleSetPoint() {}
    ~NSingleSetPoint();
    bool powered_up() const { return powered_up_; }
    bool sign() const { return sign_; }
    double value() const { return value_; }
    void set_powered_down() { powered_up_ = false; }

private:
    friend bool operator==(const NSingleSetPoint &c1, const NSingleSetPoint& c2);
    bool powered_up_;
    bool sign_;
    double value_;
};

bool operator==(const NSingleSetPoint &c1, const NSingleSetPoint& c2);
bool operator!=(const NSingleSetPoint &c1, const NSingleSetPoint& c2);

}

Q_DECLARE_METATYPE(medicyc::cyclotroncontrolsystem::hardware::nsingle::NSingleSetPoint)

#endif
