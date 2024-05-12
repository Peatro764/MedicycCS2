#include "ClickableFrame.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::multiplexed {

ClickableFrame::ClickableFrame(QWidget *parent)
 : QFrame(parent)
{
    QObject::connect( this, SIGNAL( clicked() ), this, SLOT( slotClicked() ) );
}

void ClickableFrame::slotClicked()
{
    qDebug()<<"Clicked";
}

void ClickableFrame::mousePressEvent ( QMouseEvent * event )
{
    (void)event;
    emit clicked();
}

} // ns
