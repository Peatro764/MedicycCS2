#ifndef UtilsGui_H
#define UtilsGui_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QGridLayout>
#include <QStateMachine>

#include "MonitoredState.h"

namespace Ui {
class UtilsGui;
}

namespace medicyc::cyclotroncontrolsystem::global {

class UtilsGui : public QWidget
{
    Q_OBJECT

public:
    explicit UtilsGui(QWidget *parent = 0);
    ~UtilsGui();

signals:
    void Transition();
    void Reset();

private slots:

private:
    void SetupStateMachine();
    Ui::UtilsGui *ui_;

    QStateMachine sm_;
};

}

#endif
