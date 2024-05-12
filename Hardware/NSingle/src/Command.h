#ifndef NSINGLE_COMMAND_H
#define NSINGLE_COMMAND_H

#include "NSingle_global.h"

#include <QObject>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::nsingle {

class NSINGLE_EXPORT Command
{
public:
    Command(QString header, QString body, QString footer);
    Command(QString header, QString body, QString footer, int execution_time);
    QString Packaged() const;
    bool Reply() const { return reply_; }
    int ExecutionTime() const { return execution_time_; }

private:
    QString header_;
    QString body_;
    QString footer_;
    bool reply_ = true;
    int execution_time_ = 0;
};

} // namespace

#endif
