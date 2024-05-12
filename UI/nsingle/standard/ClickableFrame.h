#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

class ClickableFrame : public QFrame
{
    Q_OBJECT

public:
    ClickableFrame(QWidget *parent = 0);
signals:
    void clicked();

public slots:
    void slotClicked();

protected:
    void mousePressEvent ( QMouseEvent * event ) ;

};

} // ns

#endif // CLICKABLEFRAME_H
