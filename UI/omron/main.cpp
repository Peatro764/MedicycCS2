#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "OmronGui.h"
#include "Parameters.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    if (QCoreApplication::arguments().size() != 2) {
        qWarning() << "Wrong number of arguments given. Format: ./OmronGui \"HF\"";
        return -1;
    }

    const QString system = QCoreApplication::arguments().at(1);
    qDebug() << "Setting up OMRON Gui " << system;

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::ui::omron::OmronGui gui(system);
    gui.show();
    return app.exec();
}


