#include <QApplication>
#include <QDebug>

#include "DeeVoltageGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::DeeVoltageGui gui(0);
    gui.show();
    return app.exec();
}


