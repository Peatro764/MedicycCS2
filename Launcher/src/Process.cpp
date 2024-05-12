#include "Process.h"


namespace medicyc::cyclotroncontrolsystem::launcher {

Process::Process() {
}


Process::Process(QString id, QString path, QString name, QStringList args, std::vector<Library> libs, int priority, bool required, QString logdir) :
    id_(id), path_(path), name_(name), args_(args), libs_(libs), priority_(priority), required_(required), logdir_(logdir) {
}

Process::~Process() {

}

QString Process::ld_library_paths() const {
    QStringList paths;
    for (auto& lib : libs_) {
        paths.append(lib.path());
    }
    return paths.join(":");
}


bool operator==(const Process &c1, const Process& c2) {
    return (c1.id_ == c2.id_ &&
            c1.path_ == c2.path_ &&
            c1.name_ == c2.name_ &&
            c1.args_ == c2.args_ &&
            c1.libs_ == c2.libs_ &&
            c1.priority_ == c2.priority_ &&
            c1.required_ == c2.required_ &&
            c1.logdir_ == c2.logdir_);
}

bool operator!=(const Process &c1, const Process& c2) {
    return !(c1 == c2);
}

}
