#include "PoleLevelIndicator.h"

#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <algorithm>

PoleLevelIndicator::PoleLevelIndicator(QWidget *parent, int width, int height, int n_step, double max)
    : QWidget(parent), n_steps_(n_step), max_(max)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(width, height);
    SetupLevels();
}

void PoleLevelIndicator::Configure(double n_steps, double max) {
    value_ = 0.0;
    n_steps_ = n_steps;
    max_ = max;
    SetupLevels();
    update();
}


void PoleLevelIndicator::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    DrawPole(&painter);
}

void PoleLevelIndicator::SetupLevels() {
    levels_.clear();
    colors_.clear();
    if (n_steps_ < 1) {
        qWarning() << "PoleLevelIndicator::SetupLevels Nsteps must be atleast 1. Forcing it to 1.";
        n_steps_ = 1;
        levels_.push_back(max_);
        colors_.push_back(Qt::yellow);
    } else {
        const double delta = max_ / static_cast<double>(n_steps_);
        for (int i = 0; i < n_steps_; ++i) {
            double level = static_cast<double>(i) * delta;
            levels_.push_back(level);
            colors_.push_back(GetColor(i));
        }
    }
}

QColor PoleLevelIndicator::GetColor(int step) const {
    const int shades_per_base_colors = n_steps_ / base_colors_.size();
    QColor base_color = base_colors_.at(step / shades_per_base_colors);
    return base_color.darker(100 + 30 * (step % shades_per_base_colors)); // arg = 100 returns same color
}

void PoleLevelIndicator::SetLevel(double value) {
    value_ = value;
    update();
}

void PoleLevelIndicator::SetEnabled(bool flag) {
    enabled_ = flag;
    update();
}

void PoleLevelIndicator::DrawPole(QPainter *painter) {
    const int w = width();
    const int h = height();
    QPen pen;
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(1);
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    if (enabled_) {
        double pole_element_height = h / n_steps_;
        pen.setColor(Qt::black);
        for (int i = 0; i < n_steps_; ++i) {
            QColor color = (value_ >= levels_.at(i) ? GetColor(i) : COLOR_DISABLED);
            brush.setColor(color);
            painter->setBrush(brush);
            painter->setPen(pen);
            painter->drawRect(0, h - (i + 1) * pole_element_height, w, pole_element_height);
        }
    } else {
        pen.setColor(COLOR_DISABLED);
        brush.setColor(COLOR_DISABLED);
        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRect(0, 0, w, h);
    }
}

