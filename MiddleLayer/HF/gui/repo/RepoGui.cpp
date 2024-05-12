#include "RepoGui.h"

#include <QDebug>
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::middlelayer::hf  {

RepoGui::RepoGui() :
    repo_("_prod")
{
    ConnectSignals();
}

RepoGui::~RepoGui()
{
}

void RepoGui::ConnectSignals() {
    QObject::connect(&repo_, &HFRepo::SIGNAL_NewConfigurationAdded, this, [&](int id) {
        qDebug() << "Notification " << id;
        Configuration config = repo_.GetConfiguration(id);
        qDebug() << "Got configuration " << config.timestamp();
    });
}

}
