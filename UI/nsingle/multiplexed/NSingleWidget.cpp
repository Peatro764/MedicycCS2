#include "NSingleWidget.h"

#include <QLabel>
#include <QFrame>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::multiplexed {

NSingleWidget::NSingleWidget(QString name) :
    name_(name)
{
    layout_ = CreateLayout();
    SetupConnections();
}

void NSingleWidget::SetupConnections() {
    QObject::connect(frame_connection_, &ClickableFrame::clicked, this, &NSingleWidget::Toggle);
}

QVBoxLayout* NSingleWidget::CreateLayout() {
    layout_ = new QVBoxLayout;
    label_ = new QLabel(name_);
    QFont font("Ubuntu", 9);
    font.setBold(true);
    label_->setFont(font);
    label_->setAlignment(Qt::AlignCenter);
    frame_connection_ = new ClickableFrame;
    frame_connection_->setFixedSize(QSize(120, 120));
    frame_connection_->setObjectName("frame_connection");
    Disconnected();

    frame_ping_ = new QFrame(frame_connection_);
    frame_ping_->setGeometry(QRect(QPoint(4, 4), QSize(20, 20)));
    frame_ping_->setFixedSize(QSize(20, 20));
    frame_ping_->setObjectName("frame_ping");
    frame_ping_->setStyleSheet(QString("QFrame#") +frame_ping_->objectName() + "{}");

    layout_->addWidget(label_);
    layout_->addWidget(frame_connection_);
    return layout_;
}

void NSingleWidget::Connected() {
    connected_ = true;
    Draw();
}

void NSingleWidget::Disconnected() {
    connected_ = false;
    Draw();
}

void NSingleWidget::Toggle() {
    selected_ = !selected_;
    if (selected_) {
        emit Selected(name_);
    } else {
        emit Deselected(name_);
    }
    Draw();
}

void NSingleWidget::Select() {
    selected_ = true;
    emit Selected(name_);
    Draw();
}

void NSingleWidget::Deselect() {
    selected_ = false;
    emit Deselected(name_);
    Draw();
}


void NSingleWidget::Draw() {
    QString str(QString("QFrame#") + frame_connection_->objectName() + " {");
    if (connected_) {
        str.append("background-image: url(:/Images/connected_120x120.png);");
    } else {
        str.append("background-image: url(:/Images/disconnected_120x120.png);");
    }
    str.append("background-repeat: no-repeat;");
    str.append("background-position: center;");
    if (selected_) {
        str.append("border: 3px solid rgb(85, 170, 255);");
    }
    str.append("}");
    frame_connection_->setStyleSheet(str);
}

void NSingleWidget::Ping() {
    QString ping_on (QString("QFrame#") + frame_ping_->objectName() + " {"
                 "background-image: url(:/Images/" + "circle-blue-20x20" + ");"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "};");
    frame_ping_->setStyleSheet(ping_on);
    QTimer::singleShot(500, this, [&]() { frame_ping_->setStyleSheet("QFrame {}"); });
}

} // ns

