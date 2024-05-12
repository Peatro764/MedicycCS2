#include <QtDBus/QDBusConnection>
#include <QtWidgets/QApplication>
#include <QtDBus>

#include "Source.h"
//#include "Arc.h"
//#include "Extraction.h"
//#include "Filament.h"

#include "source_adaptor.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDBusConnection connection = QDBusConnection::sessionBus();
    if (connection.isConnected()) {
        qWarning() << "Connection to dbus successful.";
    } else {
        qWarning() << "Connection to dbus failed.";
        return -1;
    }

    medicyc::cyclotroncontrolsystem::middlelayer::source::Source source;
    new SourceInterfaceAdaptor(&source);
    connection.registerObject("/Source", &source);
    connection.registerService("medicyc.cyclotron.middlelayer.source");
    return app.exec();
}


