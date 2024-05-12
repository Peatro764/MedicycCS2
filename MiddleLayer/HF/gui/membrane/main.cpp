#include <QApplication>
#include <QDebug>

#include "MembraneGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::MembraneGui gui(0);
    gui.show();
    return app.exec();
}


