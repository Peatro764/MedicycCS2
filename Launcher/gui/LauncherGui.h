#ifndef LAUNCHERGUI_H
#define LAUNCHERGUI_H

#include <QWidget>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>
#include <QGridLayout>
#include <QPushButton>
#include <QMap>
#include <QTimer>

#include "launcher_interface.h"

namespace Ui {
class LauncherGui;
}

namespace medicyc::cyclotroncontrolsystem::launcher {

class LauncherGui : public QWidget {
    Q_OBJECT
public:
    explicit LauncherGui(QWidget *parent = 0);
     ~LauncherGui();

public slots:

private slots:
    void CreateProcessPushButton(QString name);
    //void DeleteProcessPushButton(QString name);
    void ProcessStarted(QString name);
    void ProcessStopped(QString name);

signals:

private:
    void ConnectSignals();
    void SetupLayout();
    QPushButton* GetNewButton(QString text);
    QString ButtonStyleSheet(bool on) const;

    Ui::LauncherGui *ui_;
    QGridLayout *gridLayout_real_buttons_ = nullptr;
    QGridLayout *gridLayout_sim_buttons_ = nullptr;

    const int NCOL = 4; // gridcolumns
    int n_real_processes_ = 0;
    int n_sim_processes_ = 0;

    QMap<QString, QPushButton*> process_buttons_;
    medicyc::cyclotron::LauncherInterface launcher_interface_;
};

}

#endif
