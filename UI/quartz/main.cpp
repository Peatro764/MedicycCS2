#include "Quartz.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>
#include <QSettings>
#include <QStandardPaths>

#include "Parameters.h"

int main(int argc, char *argv[])
{   
    QApplication app(argc, argv);

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
    }

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    medicyc::cyclotroncontrolsystem::ui::quartz::Quartz quartz;
//    gui.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    quartz.show();
    return app.exec();
}

