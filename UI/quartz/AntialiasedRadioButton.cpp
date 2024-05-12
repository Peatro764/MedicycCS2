#include "AntialiasedRadioButton.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionButton>

AntialiasedRadioButton::AntialiasedRadioButton(QString label)
    : QRadioButton(label)
{

}

void AntialiasedRadioButton::paintEvent(QPaintEvent* event) {
    QStylePainter p(this);
    p.setRenderHint(QStylePainter::Antialiasing, true);
    QStyleOptionButton opt;
    initStyleOption(&opt);
    p.drawControl(QStyle::CE_RadioButton, opt);
}
