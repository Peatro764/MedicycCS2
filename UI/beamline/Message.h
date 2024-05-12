#ifndef MESSAGE_H
#define MESSAGE_H

#include <QColor>
#include <QString>

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

// TODO: This is a temporary implementation to save messages. It should
// be handled by the MainGui once it is finished
class Message {
public:
    Message(QString content, QColor color) :
        content_(content), color_(color) {}
    QString content() const { return content_; }
    QColor color() const { return color_; }
private:
    QString content_;
    QColor color_;
};

}

#endif
