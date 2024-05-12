#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "UtilsGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::global::UtilsGui gui(0);
    gui.show();
    return app.exec();
}


