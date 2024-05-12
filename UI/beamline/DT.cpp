#include "DT.h"

#include <QColor>
#include <QBrush>
#include <QPainter>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

DT::DT(BeamItem item) : QGraphicsObject(), BeamItem(item),
    extract_pos_(0, 0),
    insert_pos_(0, Constants::BEAM_ITEM_HEIGHT),
    size_dt_(Constants::BEAM_ITEM_WIDTH, Constants::BEAM_ITEM_HEIGHT),
    size_boundary_(Constants::BEAM_ITEM_WIDTH, 2*Constants::BEAM_ITEM_HEIGHT),
    label_("DT", this)
{
    QFont font("Ubuntu");
    font.setPixelSize(Constants::BEAM_ITEM_FONT_SIZE);
    label_.setFont(font);
    label_.setDefaultTextColor(Qt::yellow);
    label_.moveBy(-size_dt_.width(), Constants::BEAM_ITEM_HEIGHT + 2*Constants::PIPE_OUTER_RADIUS);
    pos_dt_ = extract_pos_;
    anim_ = new QPropertyAnimation(this, "pos_dt");
    anim_->setDuration(500);
    update();
}

QRectF DT::boundingRect() const
{
    return QRectF(pos_boundary_, size_boundary_);
}

void DT::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    (void)option;
    (void)widget;

    QPen pen(Qt::transparent);
    pen.setWidth(0);

    // Boundary
    QRectF rect_boundary(pos_boundary_, size_boundary_);
    painter->setPen(pen);
    painter->setBrush(Qt::transparent);
    painter->drawRect(rect_boundary);

    // Beam pipe
    pen.setBrush(QColor("#DBE2E9"));
    pen.setColor(QColor("#DBE2E9"));
    painter->setPen(pen);
    painter->setBrush(QColor("#DBE2E9"));
    const int size = (Constants::PIPE_OUTER_RADIUS - Constants::PIPE_INNER_RADIUS) - 3;
    QRectF pipe_left(rect_boundary.topLeft() - QPointF(size, 1), QSize(size, size_dt_.height()));
    QRectF pipe_top(rect_boundary.topLeft() - QPoint(size, size+1), QSize(size_dt_.width() + size*2, size));
    QRectF pipe_right(rect_boundary.topRight() - QPoint(0, 1) , QSize(size, size_dt_.height()));
    painter->drawRect(pipe_left);
    painter->drawRect(pipe_top);
    painter->drawRect(pipe_right);
    QRectF pipe_bottom(rect_boundary.bottomLeft() - QPointF(-1, size_dt_.height() + size + 1), QSize(size_dt_.width() - 2, size + 1));
    painter->setBrush(Qt::black);
    pen.setColor(Qt::black);
    pen.setBrush(Qt::black);
    painter->setPen(pen);
    painter->drawRect(pipe_bottom);

    // DT
    pen.setColor(Qt::black);
    painter->setPen(pen);
    QRectF rect_cf(pos_dt_, size_dt_);
    painter->setBrush(QColor("grey"));
    painter->drawRect(rect_cf);
}

bool DT::SetState(bool insert) {
    bool state_changed = (insert != inserted_);
    inserted_ = insert;
    anim_->setStartValue(pos_dt_);
    anim_->setEndValue(insert ? insert_pos_ : extract_pos_);
    anim_->start();
    if (state_changed) PlayStateChange(inserted_);
    return state_changed;
}

void DT::PlayStateChange(bool inserted) {
    if (inserted) {
        QSound::play(":/sounds/item_inserted.wav");
    } else {
        QSound::play(":/sounds/item_extracted.wav");
    }
}

}
