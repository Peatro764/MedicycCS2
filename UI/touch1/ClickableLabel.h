#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    ClickableLabel(QString text);
signals:
    void SIGNAL_Clicked();

public slots:

protected:
    void mousePressEvent ( QMouseEvent * event ) ;

};

} // ns

#endif // CLICKABLELABEL_H
