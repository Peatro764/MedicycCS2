#include <QApplication>
#include <QDebug>

#include "AccordGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::AccordGui gui(0);
    gui.show();
    return app.exec();
}


