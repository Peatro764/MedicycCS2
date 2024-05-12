#include <QApplication>
#include <QDebug>

#include "ControllerGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::ControllerGui gui(0);
    gui.show();
    return app.exec();
}


