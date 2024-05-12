#include "BitFieldWidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

BitFieldWidget::BitFieldWidget(QWidget *parent) : QWidget(parent) {
}

void BitFieldWidget::Setup(int nbitsmax, int nbits, bool read_only, BITFIELDALIGNMENT alignment) {
    TearDown();
    show();
    const int diameter =  std::min(rect().height(), rect().width() / nbitsmax);
    for (int iBit = 0; iBit < nbits; ++iBit) {
        BitWidget *bw = new BitWidget(read_only, this);
        bw->resize(diameter, diameter);
        const int y_offset =  (rect().height() - diameter) / 2;
        if (alignment == BITFIELDALIGNMENT::RIGHT) {
            bw->move(width() - (iBit + 1)*diameter, y_offset);
        } else if (alignment == BITFIELDALIGNMENT::LEFT) {
            bw->move((nbits - iBit - 1) * diameter, y_offset);
        } else { // SPARSE or other
            const int inter_spacing = (rect().width() - nbits * diameter) / (nbits + 1);
            bw->move(iBit * diameter + (iBit + 1) * inter_spacing, y_offset);
        }
        QObject::connect(bw, &BitWidget::SIGNAL_Changed, this, &BitFieldWidget::Changed);
        bits_[iBit] = bw;
        bw->show();
    }
}

void BitFieldWidget::TearDown() {
    for (auto& key : bits_.keys()) {
        delete bits_.value(key);
    }
    bits_.clear();
}

void BitFieldWidget::Enable(int bit) {
    if (bits_.contains(bit)) {
        bits_.value(bit)->SetEnabled(true);
    } else {
        qDebug() << "BitFieldWidget::Enable Unknown bit " << bit;
    }
}

void BitFieldWidget::Disable(int bit) {
    if (bits_.contains(bit)) {
        bits_.value(bit)->SetEnabled(false);
    } else {
        qDebug() << "BitFieldWidget::Disable Unknown bit " << bit;
    }
}

void BitFieldWidget::Changed() {
    const int value = GetValue();
    emit SIGNAL_Changed(value);
}

int BitFieldWidget::GetValue() const {
    int value(0);
    for (auto& key : bits_.keys()) {
        value += (bits_.value(key)->On() ? 1 << key : 0);
    }
    return value;
}

void BitFieldWidget::SetValue(int value) {
    for (auto& key : bits_.keys()) {
        if ((value & (0x1 << key)) != bits_.value(key)->On()) {
            bits_.value(key)->SetOn(value & (0x1 << key));
        }
    }
}

void BitFieldWidget::paintEvent(QPaintEvent *event) {
    (void)event;
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QColor::fromRgb(0x31, 0x36, 0x3b));
    painter.drawRect(rect());
    painter.end();
}

} // ns

