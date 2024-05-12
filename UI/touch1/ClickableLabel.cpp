#include "ClickableLabel.h"

#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

ClickableLabel::ClickableLabel(QString text)
 : QLabel(text)
{
}

void ClickableLabel::mousePressEvent ( QMouseEvent * event )
{
    (void)event;
    qDebug() << "ClickableLabel::mousePressEvent";
    emit SIGNAL_Clicked();
}

} // ns
