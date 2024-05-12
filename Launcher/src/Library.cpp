#include "Library.h"


namespace medicyc::cyclotroncontrolsystem::launcher {

Library::Library() {}

Library::Library(QString id, QString path, QString name) :
    id_(id), path_(path), name_(name) {
}

Library::~Library() {

}

bool operator==(const Library &c1, const Library& c2) {
    return (c1.id_ == c2.id_ &&
            c1.path_ == c2.path_ &&
            c1.name_ == c2.name_);
}

bool operator!=(const Library &c1, const Library& c2) {
    return !(c1 == c2);
}

}
