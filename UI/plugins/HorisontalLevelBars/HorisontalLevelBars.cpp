#include "HorisontalLevelBars.h"

#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <algorithm>
#include <QPainterPath>

HorisontalLevelBars::HorisontalLevelBars(QWidget *parent, int width, int height, QBrush on, QBrush off, std::vector<int> steps)
    : QWidget(parent), brush_on_(on), brush_off_(off), steps_(steps)
{
    assert(steps.size() > 1);
    setFixedSize(width, height);
    QObject::connect(this, &HorisontalLevelBars::SIGNAL_Clicked, this, &HorisontalLevelBars::DoOneStep);
}

void HorisontalLevelBars::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    DrawBars(&painter);
}

void HorisontalLevelBars::ResetLevel() {
    act_step_ = 0;
    emit SIGNAL_ActiveLevelChanged(steps_.at(act_step_));
    update();
}

void HorisontalLevelBars::DoOneStep() {
    if (act_step_ >= static_cast<int>((steps_.size() - 1))) {
        act_step_ = 0;
    } else {
        act_step_++;
    }
    emit SIGNAL_ActiveLevelChanged(steps_.at(act_step_));
    update();
}

void HorisontalLevelBars::DrawBars(QPainter *painter) {
    const int w = width();
    const int h = height();

    int nsteps = static_cast<int>(steps_.size());
    float bar_height = h / (steps_.size() + 1);
    float bar_width = w;
    float bar_intra_height = (h - steps_.size()*bar_height) / (nsteps - 1);

    QBrush brush;
    for (int bar = 0; bar < nsteps; ++bar) {
        if (act_step_ >= bar) {
            brush = brush_on_;
        } else {
            brush =  brush_off_;
        }
        painter->setBrush(brush);
//        painter->drawRect(0, h - bar*bar_height - (bar + 1)*bar_intra_height - bar_height/2, bar_width, bar_height);

        painter->setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addRoundedRect(QRectF(0, h - bar*bar_height - (bar + 1)*bar_intra_height - bar_height/2, bar_width, bar_height), 4, 4);
//        p.fillPath(path, Qt::red);
        painter->drawPath(path);
    }    
}

void HorisontalLevelBars::mousePressEvent ( QMouseEvent * event )
{
    (void)event;
    emit SIGNAL_Clicked();
}


