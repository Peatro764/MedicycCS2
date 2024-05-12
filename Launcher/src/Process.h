#ifndef LAUNCHER_PROCESS_H
#define LAUNCHER_PROCESS_H

#include <QObject>
#include <QString>
#include <QDebug>

#include "Library.h"

namespace medicyc::cyclotroncontrolsystem::launcher {

class Process {
public:
    Process();
    Process(QString id, QString path, QString name, QStringList args, std::vector<Library> libs, int priority, bool required, QString logdir);
    ~Process();
    QString id() const { return id_; }
    QString path() const { return path_; }
    QString name() const { return name_; }
    QStringList args() const { return args_; }
    std::vector<Library> libs() const { return libs_; }
    QString ld_library_paths() const;
    int priority() const { return priority_; }
    bool required() const { return required_; }
    QString logdir() const { return logdir_; }

private:
    friend bool operator==(const Process &c1, const Process& c2);
    QString id_;
    QString path_;
    QString name_;
    QStringList args_;
    std::vector<Library> libs_;
    int priority_;
    bool required_;
    QString logdir_;
};

bool operator==(const Process &c1, const Process& c2);
bool operator!=(const Process &c1, const Process& c2);

}

Q_DECLARE_METATYPE(medicyc::cyclotroncontrolsystem::launcher::Process)

#endif
