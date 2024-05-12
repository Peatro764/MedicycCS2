#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QStandardPaths>
#include <QDebug>

#include "CycloConfig.h"

using namespace cyclo_config;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);    

    CycloConfig config;

    return app.exec();

}


