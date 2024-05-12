#include "StateLabel.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

StateLabel::StateLabel(QString title, QWidget* parent)
    : QLabel(title, parent)
{
    QFont font;
    font.setPointSize(13);
    font.setBold(false);
    setMinimumHeight(30);
    setFont(font);
    setAlignment(Qt::AlignCenter);
    setObjectName(title);
}

void StateLabel::SetGreen() {
    SetStyleSheet("green");
}

void StateLabel::SetYellow() {
    SetStyleSheet("#8B8000");
}

void StateLabel::SetRed() {
    SetStyleSheet("red");
}

void StateLabel::SetBlack() {
    SetStyleSheet("black");
}

void StateLabel::SetStyleSheet(QString color) {
    QString stylesheet;
    stylesheet = "QWidget#" + this->objectName() + "{" +
            "background-color: qlineargradient(x1: 0, y1: 0.4, x2: 0, y2: 1.0, stop: 0 " + color + ", stop: 1 " + "grey" + ");" +
            "border-style: solid;" +
            "border-color: white;" +
            "border-width: 1px;" +
            "border-radius: 3px;" + "}";
    setStyleSheet(stylesheet);
}


}

