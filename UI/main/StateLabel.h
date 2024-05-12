#ifndef STATELABEL_H
#define STATELABEL_H

#include <QLabel>

namespace medicyc::cyclotroncontrolsystem::ui::main {

class StateLabel : public QLabel
{
    Q_OBJECT
public:
    StateLabel(QString label, QWidget* parent = nullptr);
public slots:
    void SetGreen();
    void SetYellow();
    void SetRed();
    void SetBlack();
private:
    void SetStyleSheet(QString color);
};

} // ns

#endif // STATELABEL_H
