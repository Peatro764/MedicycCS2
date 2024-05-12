#include <QApplication>
#include <QDebug>

#include "HFGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::hardware::hfpowersupplies::HFGui gui(0);
    gui.show();
    return app.exec();
}


