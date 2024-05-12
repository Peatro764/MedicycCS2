#include <QApplication>
#include <QDebug>

#include "NiveauGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::NiveauGui gui(0);
    gui.show();
    return app.exec();
}


