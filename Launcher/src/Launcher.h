#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QString>
#include <QDateTime>
#include <map>
#include <memory>
#include <QStandardPaths>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTimer>
#include <QThread>

#include "Process.h"
#include "Library.h"

namespace medicyc::cyclotroncontrolsystem::launcher {

struct DatabaseConfig {
    QString host_name;
    QString database_name;
    int port;
    QString username;
    QString password;
    int connection_timeout;
};

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(DatabaseConfig config, QObject *parent = 0);
    ~Worker();

public slots:
  void GetProcesses();
  void GetRealProcesses();
  void GetRealRequiredProcesses();
  void StartRealProcesses();
  void StartRealRequiredProcesses();
  void StartSimProcesses();
  void StopProcesses();
  void StartProcess(QString name);
  void StopProcess(QString name);
  void ToggleProcessState(QString name);
  void RestartProcess(QString name);

private slots:
    void CreateProcesses();
    void DeleteProcesses();
    QString GetTopDir();
    QString GetLogDir();
    std::vector<Library> GetLibraries(QString process_id);
    QList<launcher::Process> QueryDbForProcesses();
    void PostMortem(QString name);

signals:
    void SIGNAL_Error(QString message);
    void SIGNAL_ProcessCreated(QString name);
    void SIGNAL_ProcessDeleted(QString name);
    void SIGNAL_ProcessStarted(QString name);
    void SIGNAL_ProcessStopped(QString name);
    void SIGNAL_AllRequiredNotRunning();
    void SIGNAL_AllRequiredRunning();

private:
    void ConnectSignals();
    QStringList DecodeStringArray(QString array);
    void CheckRequiredRunning();

    DatabaseConfig config_;
    QMap<QString, QProcess*> processes_;
    QMap<QString, bool> required_processes_running_;
    QString top_dir_ = "";
    QString log_dir_ = "";
    const QString sim_prefix_ = "Sim_";
};

class Launcher : public QThread {
    Q_OBJECT
public:
    Launcher(QString name);
     ~Launcher();

public slots:
    void GetProcesses();
    void GetRealProcesses();
    void GetRealRequiredProcesses();
    void StartRealProcesses();
    void StartRealRequiredProcesses();
    void StartSimProcesses();
    void StopProcesses();
    void StartProcess(QString name);
    void StopProcess(QString name);
    void ToggleProcessState(QString name);
    void RestartProcess(QString name);

signals:  // public
    void SIGNAL_Error(QString message);
    void SIGNAL_ProcessCreated(QString name);
    void SIGNAL_ProcessDeleted(QString name);
    void SIGNAL_ProcessStarted(QString name);
    void SIGNAL_ProcessStopped(QString name);
    void SIGNAL_AllRequiredNotRunning();
    void SIGNAL_AllRequiredRunning();

signals: // private
    void SIGNAL_Forward_GetProcesses();
    void SIGNAL_Forward_GetRealProcesses();
    void SIGNAL_Forward_GetRealRequiredProcesses();
    void SIGNAL_Forward_StartRealProcesses();
    void SIGNAL_Forward_StartRealRequiredProcesses();
    void SIGNAL_Forward_StartSimProcesses();
    void SIGNAL_Forward_StopProcesses();
    void SIGNAL_Forward_StartProcess(QString name);
    void SIGNAL_Forward_StopProcess(QString name);
    void SIGNAL_Forward_ToggleProcessState(QString name);
    void SIGNAL_Forward_RestartProcess(QString name);

private:
    void CreateProcesses();
    void DeleteProcesses();
    void KillAnyExistingProcess(QString full_command);
    void run() override;
    Worker* worker_ = nullptr;
    DatabaseConfig config_;
};

}

#endif
