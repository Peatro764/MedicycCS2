#include <QApplication>
#include <QDebug>

#include "RepoGui.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    medicyc::cyclotroncontrolsystem::middlelayer::hf::RepoGui gui;
    return app.exec();
}


