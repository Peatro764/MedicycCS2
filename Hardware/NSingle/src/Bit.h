#ifndef NSINGLE_BIT_H
#define NSINGLE_BIT_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT Bit {
public:
    Bit() {}
    Bit(QString s) {
        enabled_ = !s.isEmpty();
        if (enabled_) {
            QString logic_char = s.mid(0, 1);
            if (logic_char != QString("+") && logic_char != QString("-")) {
                throw std::runtime_error("Logic char corrupt");
            }
            logic_ = logic_char == QString("+");

            QString index_char = s.mid(1, 1);
            bool ok(false);
            index_ = index_char.toInt(&ok);
            if (!ok) {
                throw std::runtime_error("Index char corrupt");
            }
        }
    }

    Bit(bool enabled, bool logic, int index)
        : enabled_(enabled), logic_(logic), index_(index) {}
    bool enabled() const  { return enabled_; }
    bool logic() const { return logic_; }
    int index() const { return index_; }

private:
    friend bool operator==(const Bit &c1, const Bit& c2);

    bool enabled_ = false;
    bool logic_ = false;
    int index_ = 0;
};

bool operator==(const Bit &c1, const Bit& c2);
bool operator!=(const Bit &c1, const Bit& c2);

} // namespace

#endif
