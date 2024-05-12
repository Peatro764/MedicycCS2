#ifndef COMMAND_H
#define COMMAND_H

#include <QObject>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::hardware::ior {

class Command
{
public:
    Command(QString header, QString body, QString footer);
    QString Packaged() const;

private:
    QString header_;
    QString body_;
    QString footer_;
};

}

#endif
