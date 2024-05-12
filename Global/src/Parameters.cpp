#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::global {

bool VerifyConfigurationFile() {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation,
                                              medicyc::cyclotroncontrolsystem::global::CONFIG_FILE,
                                              QStandardPaths::LocateFile), QSettings::IniFormat);
    if (QFile(settings.fileName()).exists()) {
        qDebug() << medicyc::cyclotroncontrolsystem::global::CONFIG_FILE << " found";
        return true;
    } else {
        qWarning() << medicyc::cyclotroncontrolsystem::global::CONFIG_FILE << " not found";
        return false;
    }
}

}
