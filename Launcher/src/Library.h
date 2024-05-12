#ifndef LAUNCHER_LIBRARY_H
#define LAUNCHER_LIBRARY_H

#include <QObject>
#include <QString>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::launcher {

class Library {
public:
    Library();
    Library(QString id, QString path, QString name);
    ~Library();
    QString id() const { return id_; }
    QString path() const { return path_; }
    QString name() const { return name_; }

private:
    friend bool operator==(const Library &c1, const Library& c2);
    QString id_;
    QString path_;
    QString name_;
};

bool operator==(const Library &c1, const Library& c2);
bool operator!=(const Library &c1, const Library& c2);

}

Q_DECLARE_METATYPE(medicyc::cyclotroncontrolsystem::launcher::Library)

#endif
