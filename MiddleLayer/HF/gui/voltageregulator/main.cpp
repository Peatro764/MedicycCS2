#include <QApplication>
#include <QDebug>

#include "VoltageRegulatorGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::VoltageRegulatorGui gui(0);
    gui.show();
    return app.exec();
}


