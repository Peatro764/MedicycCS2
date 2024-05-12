#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "NSingleGui.h"
#include "Parameters.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::ui::nsingle::standard::NSingleGui gui(0);
    gui.show();
    return app.exec();
}


