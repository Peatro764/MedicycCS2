#include "BitWidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

BitWidget::BitWidget(bool read_only, QWidget *parent) : QWidget(parent), read_only_(read_only)
{
    SetEnabled(true);
    SetOn(false);
}

bool BitWidget::On() const {
    return on_;
}

void BitWidget::SetEnabled(bool yes) {
    enabled_ = yes;
    on_ = false;
    this->update();
}

void BitWidget::SetOn(bool yes) {
    on_ = yes;
    this->update();
}

void BitWidget::mousePressEvent ( QMouseEvent * event ) {
    if (read_only_) return;

    (void)event;
    if (enabled_) {
        on_ = !on_;
        this->update();
        emit SIGNAL_Changed(on_);
    }
}

void BitWidget::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor background = QColor::fromRgb(0x31, 0x36, 0x3b);

    // Outer
    QPen pen;
    pen.setColor(Qt::lightGray);
    pen.setWidth(1);
    QColor color;
    painter.setPen(pen);
    painter.setBrush(background);
    auto outer_rect = this->rect();
    outer_rect.adjust(5, 5, -5, -5);
    painter.drawEllipse(outer_rect);

    // Inner
    if (enabled_) {
        if (on_) {
            color = QColor::fromRgb(85, 170, 255);
        } else {
            color = background;
        }
    } else {
        color = Qt::red;
    }
    pen.setWidth(0);
    pen.setColor(color);
    painter.setPen(pen);
    painter.setBrush(color);
    auto inner_rect = this->rect();
    int d = 7;
    inner_rect.adjust(d, d, -d, -d);
    painter.drawEllipse(inner_rect);
    painter.end();
}

} // ns

