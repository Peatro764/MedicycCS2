#ifndef MEDICYCCS2_GLOBAL_PARAMETERS_H
#define MEDICYCCS2_GLOBAL_PARAMETERS_H

#include <QString>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QFile>

namespace medicyc::cyclotroncontrolsystem::global {

const static QString CONFIG_FILE = "Cyclotron.ini";
bool VerifyConfigurationFile();

}

#endif // PARAMETERS_H
