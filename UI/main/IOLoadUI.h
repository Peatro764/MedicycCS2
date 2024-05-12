#ifndef IOLOADUI_H
#define IOLOADUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>
#include <QGridLayout>

#include "Style.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class IOLoadUI : public QWidget
{
    Q_OBJECT
public:
    IOLoadUI(QWidget *parent);
    ~IOLoadUI();

signals:
    void SIGNAL_CheckDbusConnection();

private slots:
    QGridLayout* SetupIOLoadMarkers();

private:
};

}

#endif
