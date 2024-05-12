#ifndef RASPBERRYGUI_H
#define RASPBERRYGUI_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>

#include "GPIOHandler.h"


namespace Ui {
class RaspberryGui;
}

namespace medicyc::cyclotroncontrolsystem::hardware::raspberry
{

class RaspberryGui : public QWidget
{
    Q_OBJECT

public:
    explicit RaspberryGui(QWidget *parent = 0);
    ~RaspberryGui();

public slots:
    void Start();

private slots:

private:
    Ui::RaspberryGui *ui_;
    GPIOHandler gpio_handler_;
};

}

#endif
