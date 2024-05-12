#include "Utils.h"

namespace shared {

namespace utils {

QPushButton* GetPushButton(QString title, int fixed_height) {
    QPushButton *pushButton = new QPushButton();
    pushButton->setText(title);
    pushButton->setFixedHeight(fixed_height);
    return pushButton;
}

QPushButton* GetPushButton(QString title, QFont font, int min_height) {
    QPushButton *pushButton = new QPushButton();
    pushButton->setFont(font);
    pushButton->setMinimumHeight(min_height);
    pushButton->setText(title);
    pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return pushButton;
}

QLabel* GetLabel(QString title, QFont font, QString color) {
    QLabel *l = new QLabel(title);
    l->setAttribute(Qt::WA_TranslucentBackground);
    l->setStyleSheet("QLabel { color : " + color + "; }");
    l->setFont(font);
    return l;
}

QLineEdit* GetLineEdit(QString title, QFont font, QString color) {
    QLineEdit *l = new QLineEdit(title);
    l->setAttribute(Qt::WA_TranslucentBackground);
    l->setStyleSheet("QLineEdit { color : " + color + "; }");
    l->setFont(font);
    return l;
}

QDoubleSpinBox* GetDoubleSpinBox(double step, double decimals, double value, double min, double max) {
    QDoubleSpinBox *s = new QDoubleSpinBox();
    s->setAlignment(Qt::AlignCenter);
    s->setValue(value);
    s->setSingleStep(step);
    s->setDecimals(decimals);
    s->setMinimum(min);
    s->setMaximum(max);
    return s;
}


void SetBorderColor(QWidget* widget, QColor color, int pixels) {
    QString stylesheet;
    stylesheet = "QWidget#" + widget->objectName() + "{" +
               + "border: " + QString::number(pixels)+ "px solid " + color.name() + ";}";
    widget->setStyleSheet(stylesheet);
}

void SetGroupStyleSheet(QWidget* widget) {
    QString stylesheet;
    stylesheet = "QWidget#" + widget->objectName() + "{" +
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 black, stop: 1 cornflowerblue);" +
               + "border-radius: 7px;" + "}";
    widget->setStyleSheet(stylesheet);
}

void SetGroupStyleSheet(QWidget* widget, QString color_hex) {
    QString stylesheet;
    stylesheet = "QWidget#" + widget->objectName() + "{" +
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 black, stop: 1 " + color_hex +");" +
               + "border-radius: 7px;" + "}";
    widget->setStyleSheet(stylesheet);
}

void SetButtonStyleSheet(QWidget *widget, QString color1, QString color2) {
    QString stylesheet;
    stylesheet = "QWidget#" + widget->objectName() + "{" +
            "background-color: qlineargradient(x1: 0, y1: 0.4, x2: 0, y2: 1.0, stop: 0 " + color1 + ", stop: 1 " + color2 + ");" +
            "border-style: solid;" +
            "border-color: white;" +
            "border-width: 1px;" +
            "border-radius: 3px;" + "}";
    widget->setStyleSheet(stylesheet);
}

QString GetButtonStyleSheet(bool on) {
    QString stylesheet;
    stylesheet.push_back("QPushButton {");
    stylesheet.push_back("color: white;");
    if (on) {
        stylesheet.push_back("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 rgb(138, 226, 52), stop: 0.5 rgb(78, 154, 6));");
    } else {
        stylesheet.push_back("background-color: qlineargradient(x1: 0.5, y1: 0.5 x2: 0.5, y2: 1, stop: 0 rgb(239, 41, 41), stop: 0.5 rgb(164, 0, 0));");
    }
    stylesheet.push_back("border-width: 0.3ex;");
    stylesheet.push_back("border-color: #76797c;");
    stylesheet.push_back("border-style: solid;");
    stylesheet.push_back("padding: 0.5ex;");
    stylesheet.push_back("border-radius: 0.2ex;");
    stylesheet.push_back("outline: none;");
    stylesheet.push_back("}");
    stylesheet.push_back("QPushButton:hover {");
    stylesheet.push_back("border: 0.4ex solid white;");
    stylesheet.push_back("}");

    return stylesheet;
}


} // ns

} // ns

