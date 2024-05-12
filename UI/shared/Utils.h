#ifndef UTILS_H
#define UTILS_H

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QWidget>

namespace shared {

namespace utils {
    QPushButton* GetPushButton(QString title, int fixed_height);
    QPushButton* GetPushButton(QString title, QFont font, int min_height);
    QLabel* GetLabel(QString title, QFont font, QString color = "White");
    QLineEdit* GetLineEdit(QString title, QFont font, QString color);
    QDoubleSpinBox* GetDoubleSpinBox(double step, double decimals, double value, double min, double max);
    void SetBorderColor(QWidget* widget, QColor color, int pixels);
    void SetGroupStyleSheet(QWidget* widget); // black -> cornflowerblue (standard)
    void SetGroupStyleSheet(QWidget* widget, QString color_hex); // black -> color_hex
    void SetButtonStyleSheet(QWidget *widget, QString color1, QString color2);
    QString GetButtonStyleSheet(bool on);
};

}

#endif // UTILS_H
