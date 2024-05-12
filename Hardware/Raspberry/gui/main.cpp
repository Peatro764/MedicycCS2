#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "raspberry.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    medicyc::cyclotroncontrolsystem::hardware::raspberry::RaspberryGui gui(0);
    gui.show();
    return app.exec();
}


