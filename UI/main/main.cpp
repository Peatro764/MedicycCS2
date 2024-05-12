#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <iostream>

#include "MainGui.h"
#include "DBus.h"
#include "Parameters.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set french language
    QTranslator qtTranslator;
    if (qtTranslator.load(QLocale::system(),
                          "qt", "_",
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qDebug() << "qtTranslator ok";
        app.installTranslator(&qtTranslator);
    }

    QTranslator qtBaseTranslator;
    if (qtBaseTranslator.load("qtbase_" + QLocale::system().name(),
                              QLibraryInfo::location(QLibraryInfo::TranslationsPath))) {
        qDebug() << "qtBaseTranslator ok";
        app.installTranslator(&qtBaseTranslator);
    }

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    if(!medicyc::cyclotroncontrolsystem::global::VerifyConfigurationFile()) {
        qWarning() << "Bailing out since no configuration file";
        return -1;
    }

    QDBusConnection dbus_connection =  medicyc::cyclotroncontrolsystem::global::GetDBusConnection();
    if (!dbus_connection.isConnected()) {
        qWarning() << "Bailing out since no dbus connection";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::ui::main::MainGui gui;
    gui.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    gui.show();
    return app.exec();
}


