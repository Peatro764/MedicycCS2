#include "CF.h"

#include <QColor>
#include <QBrush>
#include <QPainter>
#include <QSound>

#include "Constants.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

CF::CF(QString title, BeamItem item) : QGraphicsObject(), BeamItem(item),
    title_(title),
    extract_pos_(0, 0),
    insert_pos_(0, Constants::BEAM_ITEM_HEIGHT),
    size_cf_(Constants::BEAM_ITEM_WIDTH, Constants::BEAM_ITEM_HEIGHT),
    size_boundary_(Constants::BEAM_ITEM_WIDTH, 2*Constants::BEAM_ITEM_HEIGHT),
    label_(title, this)
{
    QFont font("Ubuntu");
    font.setPixelSize(Constants::BEAM_ITEM_FONT_SIZE);
    label_.setFont(font);
    label_.setDefaultTextColor(Qt::yellow);
    label_.moveBy(-size_cf_.width() / 2, Constants::BEAM_ITEM_HEIGHT + 2*Constants::PIPE_OUTER_RADIUS);
    pos_cf_ = extract_pos_;
    anim_ = new QPropertyAnimation(this, "pos_cf");
    anim_->setDuration(500);
    update();
}

QRectF CF::boundingRect() const
{
    return QRectF(pos_boundary_, size_boundary_);
}

void CF::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
    QRectF pipe_left(rect_boundary.topLeft() - QPointF(size, 1), QSize(size, size_cf_.height()));
    QRectF pipe_top(rect_boundary.topLeft() - QPoint(size, size+1), QSize(size_cf_.width() + size*2, size));
    QRectF pipe_right(rect_boundary.topRight() - QPoint(0, 1) , QSize(size, size_cf_.height()));
    painter->drawRect(pipe_left);
    painter->drawRect(pipe_top);
    painter->drawRect(pipe_right);
    QRectF pipe_bottom(rect_boundary.bottomLeft() - QPointF(-1, size_cf_.height() + size + 1), QSize(size_cf_.width() - 2, size + 1));
    painter->setBrush(Qt::black);
    pen.setColor(Qt::black);
    pen.setBrush(Qt::black);
    painter->setPen(pen);
    painter->drawRect(pipe_bottom);

    // CF
    pen.setColor(Qt::black);
    painter->setPen(pen);
    QRectF rect_cf(pos_cf_, size_cf_);
    painter->setBrush(Qt::yellow);
    painter->drawRect(rect_cf);
}

bool CF::SetState(bool insert) {
    bool state_changed = (inserted_ != insert);
    inserted_ = insert;
    anim_->setStartValue(pos_cf_);
    anim_->setEndValue(insert ? insert_pos_ : extract_pos_);
    anim_->start();
    if (state_changed) PlayStateChange(inserted_);
    return state_changed;
}

void CF::PlayStateChange(bool inserted) {
    if (inserted) {
        QSound::play(":/sounds/item_inserted.wav");
    } else {
        QSound::play(":/sounds/item_extracted.wav");
    }
}

}
