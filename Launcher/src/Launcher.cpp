#include "Launcher.h"

#include <QSettings>
#include <QSqlError>
#include <QSqlRecord>
#include <QDateTime>
#include <QTimer>
#include <QStringList>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::launcher {

Worker::Worker(DatabaseConfig config, QObject * parent) :
 QObject(parent), config_(config)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", "WorkerThread");
    db.setHostName(config.host_name);
    db.setPort(config.port);
    db.setDatabaseName(config.database_name);
    db.setUserName(config.username);
    db.setPassword(config.password);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(config_.connection_timeout));

    qDebug() << "Worker::Worker Host " << config_.host_name
             << " database " << config_.database_name
             << " port " << config_.port
             << " username " << config_.username
             << " password " << config_.password;
    ConnectSignals();
    CreateProcesses();
    QTimer::singleShot(5000, this, &Worker::StartRealRequiredProcesses);
}

Worker::~Worker() {
    QSqlDatabase::removeDatabase("WorkerThread");
    DeleteProcesses();
}

void Worker::CheckRequiredRunning() {
    if (required_processes_running_.values().contains(false)) {
        emit SIGNAL_AllRequiredNotRunning();
    } else {
        emit SIGNAL_AllRequiredRunning();
    }
}

void Worker::ConnectSignals() {
    QObject::connect(this, &Worker::SIGNAL_ProcessStopped, this, &Worker::PostMortem);
    QObject::connect(this, &Worker::SIGNAL_ProcessStopped, this, [&](QString name) {
        if (required_processes_running_.contains(name)) required_processes_running_[name] = false;
        CheckRequiredRunning();
    });
    QObject::connect(this, &Worker::SIGNAL_ProcessStarted, this, [&](QString name) {
        if (required_processes_running_.contains(name)) required_processes_running_[name] = true;
        CheckRequiredRunning();
    });
}

void Worker::PostMortem(QString name) {
    if (processes_.contains(name)) {
        qDebug() << "Worker::PostMortem " << name;
        qDebug() << "STDOUT\n " << processes_.value(name)->readAllStandardOutput();
        qDebug() << "STDERR\n " << processes_.value(name)->readAllStandardError();
    } else {
        qDebug() << "Worker::PostMortem Process not found " << name;
    }
}

void Worker::DeleteProcesses() {
    qDebug() << "Worker::DeleteProcesses";
    for(auto& name : processes_.keys()) {
        auto pr = processes_.value(name);
        pr->kill();
        delete pr;
        emit SIGNAL_ProcessDeleted(name);
    }
    processes_.clear();
}

void Worker::CreateProcesses() {
    qDebug() << "Worker::CreateProcesses";
    QList<Process> process_configs = QueryDbForProcesses();
    for (auto &p : process_configs) {
        //qDebug() << p.id() << " " << p.path() + p.name();
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        env.insert("LD_LIBRARY_PATH", p.ld_library_paths());
        QProcess *process = new QProcess(this);
        process->setProcessChannelMode(QProcess::MergedChannels);
        process->setProcessEnvironment(env);
        process->setProgram(p.path() + p.name());
        process->setArguments(p.args());
        process->setStandardOutputFile(p.logdir() + p.id());
        process->setStandardErrorFile(p.logdir() + p.id() + QString("_err"));
        emit SIGNAL_ProcessCreated(p.id());       
        QObject::connect(process, &QProcess::started, this, [&, p]() { emit SIGNAL_ProcessStarted(p.id()); });
        QObject::connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, [&, p]() { emit SIGNAL_ProcessStopped(p.id()); });
        processes_[p.id()] = process;
        if (p.required()) {
           required_processes_running_[p.id()] = false;
        }
        qDebug() << "Worker:CreateProcesses Process created " << p.name();
    }
    qDebug() << "Worker:CreateProcesses Process Done";
}

void Worker::GetProcesses() {
    qDebug() << "Worker::GetProcesses";
    for(auto &p : processes_.keys()) {
        emit SIGNAL_ProcessCreated(p);
        if (processes_.value(p)->state() != QProcess::NotRunning) {
            emit SIGNAL_ProcessStarted(p);
        }
    }
    CheckRequiredRunning();
}

void Worker::GetRealProcesses() {
    qDebug() << "Worker::GetRealProcesses";
    for(auto &p : processes_.keys()) {
        if (!p.startsWith(sim_prefix_)) {
            emit SIGNAL_ProcessCreated(p);
            if (processes_.value(p)->state() != QProcess::NotRunning) {
                emit SIGNAL_ProcessStarted(p);
            }
        } else {
            qDebug() << "Skipping " << p;
        }
    }
    CheckRequiredRunning();
}

void Worker::GetRealRequiredProcesses() {
    qDebug() << "Worker::GetRealProcesses";
    for(auto &p : processes_.keys()) {
        if (required_processes_running_.contains(p)) {
            emit SIGNAL_ProcessCreated(p);
            if (processes_.value(p)->state() != QProcess::NotRunning) {
                emit SIGNAL_ProcessStarted(p);
            }
        } else {
            qDebug() << "Skipping " << p;
        }
    }
    CheckRequiredRunning();
}

void Worker::StartRealProcesses() {
    for(auto &name : processes_.keys()) {
        if (!name.startsWith(sim_prefix_)) {
            auto process = processes_.value(name);
            QString full_command = process->program() + " " + process->arguments().join(' ');
            qDebug() << "Worker::StartRealProcesses " << full_command;
            processes_.value(name)->start();
        } else {
            qDebug() << "Skipping " << name;
        }
    }
}

void Worker::StartRealRequiredProcesses() {
    for(auto &name : processes_.keys()) {
        if (required_processes_running_.contains(name)) {
            auto process = processes_.value(name);
            QString full_command = process->program() + " " + process->arguments().join(' ');
            qDebug() << "Worker::StartRealRequiredProcesses " << full_command;
            processes_.value(name)->start();
        } else {
            qDebug() << "Worker::StartRealRequiredProcesses skipping non-required process " << name;
        }
    }
}

void Worker::StartSimProcesses() {
    for(auto &name : processes_.keys()) {
        if (name.startsWith(sim_prefix_)) {
        auto process = processes_.value(name);
        QString full_command = process->program() + " " + process->arguments().join(' ');
        qDebug() << "Worker::StartSimProcesses " << full_command;
        processes_.value(name)->start();
        } else {
            qDebug() << "Worker::StartSimNSingleProcesses skipping non-sim-single process " << name;
        }
    }
}

void Worker::StopProcesses() {
    for(auto &name : processes_.keys()) {
        qDebug() << "Worker::StopProcesses " << name;
        processes_.value(name)->kill();
    }
}

void Worker::StartProcess(QString name) {
    if (processes_.contains(name)) {
        auto process = processes_.value(name);
        QString full_command = process->program() + " " + process->arguments().join(' ');
        if (process->state() == QProcess::NotRunning) {
            qDebug() << "Worker::StartProcess Starting process " << full_command;
            process->start();
        } else {
            qDebug() << "Worker::StartProcess Process already running " << name;
        }
    } else {
        qDebug() << "Worker::StartProcess Process not found " << name;
    }
}

void Worker::ToggleProcessState(QString name) {
    if (processes_.contains(name)) {
        auto process = processes_.value(name);
        if (process->state() == QProcess::NotRunning) {
            StartProcess(name);
        } else {
            StopProcess(name);
        }
    } else {
        qDebug() << "Worker::ToggleProcessState Process not found " << name;
    }
}

void Worker::StopProcess(QString name) {
    if (processes_.contains(name)) {
        auto process = processes_.value(name);
        if (process->state() != QProcess::NotRunning) {
            qDebug() << "Worker::StopProcess Stopping process " << name;
            process->kill();
        } else {
            qDebug() << "Worker::StopProcess Process already stopped" << name;
        }
    } else {
        qDebug() << "Worker::StopProcess Process not found " << name;
    }
}

void Worker::RestartProcess(QString name) {
    if (processes_.contains(name)) {
        processes_.value(name)->kill();
        QTimer::singleShot(2000, this, [&, name](){ processes_.value(name)->start(); });
    } else {
        qDebug() << "Worker::RestartProcess Process not found " << name;
    }
}

QString Worker::GetTopDir() {
    if (!top_dir_.isEmpty()) {
        return top_dir_;
    }
    qDebug() << "Worker::GetTopDir";
    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "Worker::GetTopDir Failed open";
        emit SIGNAL_Error("Le lanceur de processus n'a pas pu se connecter à la base de données");
        return QString();
    }

    QSqlQuery query(db);
    query.prepare("SELECT topdir FROM ProcessMeta LIMIT 1");

    if (!query.exec()){
        db.close();
        qWarning() << QString("Worker::GetTopDir Query failed:") << query.lastQuery();
        return QString();
    }

    if (!query.next()) {
        db.close();
        qWarning() << QString("Worker::GetTopDir Empty result");
        return QString();
    }

    return query.value(0).toString();
}

QString Worker::GetLogDir() {
    if (!log_dir_.isEmpty()) {
        return log_dir_;
    }
    qDebug() << "Worker::GetLogDir";
    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "Worker::GetLogDir Failed open";
        emit SIGNAL_Error("Le lanceur de processus n'a pas pu se connecter à la base de données");
        return QString();
    }

    QSqlQuery query(db);
    query.prepare("SELECT logdir FROM ProcessMeta LIMIT 1");

    if (!query.exec()){
        db.close();
        qWarning() << QString("Worker::GetLogDir Query failed:") << query.lastQuery();
        return QString();
    }

    if (!query.next()) {
        db.close();
        qWarning() << QString("Worker::GetLogDir Empty result");
        return QString();
    }

    return query.value(0).toString();
}

std::vector<Library> Worker::GetLibraries(QString process_id) {
    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "Worker::GetLibraries Failed open";
        emit SIGNAL_Error("Le lanceur de processus n'a pas pu se connecter à la base de données");
        return std::vector<Library>();
    }

    top_dir_ = GetTopDir();

    QSqlQuery query(db);
    query.prepare("SELECT l.id, l.path, l.name FROM Library l "
                  "JOIN ProcessLibrary pl on l.id = pl.library_id "
                  "WHERE pl.process_id = :process_id");
    query.bindValue(":process_id", process_id);

    std::vector<Library> libraries;

    if (!query.exec()){
        db.close();
        qWarning() << QString("Worker::GetLibraries Query failed:") << query.lastQuery();
        return libraries;
    }

    while (query.next()) {
        Library library(query.value(0).toString(),
                        top_dir_ + query.value(1).toString(),
                        query.value(2).toString());
        libraries.push_back(library);
    }
    return libraries;
}

QList<Process> Worker::QueryDbForProcesses() {
    qDebug() << "Worker::QueryDbForProcesses";
    QList<Process> processes;

    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "Worker::QueryDbForProcesses Failed open";
        emit SIGNAL_Error("Le lanceur de processus n'a pas pu se connecter à la base de données");
        return processes;
    }

    top_dir_ = GetTopDir();
    log_dir_ = GetLogDir();

    QSqlQuery query(db);
    query.prepare("SELECT id, path, name, args, priority, required FROM Process "
                  "WHERE enabled = True "
                  "ORDER BY priority asc");

    if (!query.exec()){
        db.close();
        qWarning() << QString("Worker::GetProcesses Query failed:") << query.lastQuery();
        return processes;
    }

    while (query.next()) {
        const QString id = query.value(0).toString();
        const QString path = top_dir_ + query.value(1).toString();
        const QString name = query.value(2).toString();
        QStringList args = DecodeStringArray(query.value(3).toString());
        const std::vector<Library> libs = GetLibraries(query.value(0).toString());
        const int priority = query.value(4).toInt();
        bool required = query.value(5).toBool();
        Process process(id, path, name, args, libs, priority, required, log_dir_);
        processes.push_back(process);
    }
    return processes;
}

QStringList Worker::DecodeStringArray(QString array) {
    QString arrayWithoutBraces(array.remove("{").remove("}"));
    return arrayWithoutBraces.split(",", Qt::SkipEmptyParts);
}

/***
 *
 *  Launcher
 * ***/

Launcher::Launcher(QString name)
{
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    config_.host_name = settings.value(name + "/host", "unknown").toString();
    config_.database_name = settings.value(name + "/name", "unknown").toString();
    config_.port = settings.value(name + "/port", "0").toInt();
    config_.username = settings.value(name + "/user", "unknown").toString();
    config_.password = settings.value(name + "/password", "unknown").toString();
    qDebug() << "Startup";
    //        KillAnyExistingProcess("VDF");
//        KillAnyExistingProcess("Ejection");
//        KillAnyExistingProcess("Injection");
//        KillAnyExistingProcess("Bobines");
//        KillAnyExistingProcess("StandardNSingleDbus");
//        KillAnyExistingProcess("MultiplexedNSingleDbus");
//        KillAnyExistingProcess("SignalGeneratorDbus");

    this->start();
}

Launcher::~Launcher() {
    this->quit();
    while (this->isRunning()) {}
    if (worker_) delete worker_;
}

void Launcher::KillAnyExistingProcess(QString command) {
    qDebug() << "Kill " << command;
    QProcess process;
    process.setProgram("killall");
    QStringList args { command };
    process.setArguments(args);
    qDebug() << "Killing " << process.program() + " " + process.arguments().join(" <> ");
    process.start();
    process.waitForFinished(1000);
}

void Launcher::GetProcesses() {
   emit SIGNAL_Forward_GetProcesses();
}

void Launcher::GetRealProcesses() {
   emit SIGNAL_Forward_GetRealProcesses();
}

void Launcher::GetRealRequiredProcesses() {
   emit SIGNAL_Forward_GetRealRequiredProcesses();
}

void Launcher::StartRealProcesses() {
   emit SIGNAL_Forward_StartRealProcesses();
}

void Launcher::StartRealRequiredProcesses() {
   emit SIGNAL_Forward_StartRealRequiredProcesses();
}

void Launcher::StartSimProcesses() {
   emit SIGNAL_Forward_StartSimProcesses();
}

void Launcher::StopProcesses() {
   emit SIGNAL_Forward_StopProcesses();
}

void Launcher::StartProcess(QString name) {
   emit SIGNAL_Forward_StartProcess(name);
}

void Launcher::ToggleProcessState(QString name) {
   emit SIGNAL_Forward_ToggleProcessState(name);
}

void Launcher::StopProcess(QString name) {
  emit SIGNAL_Forward_StopProcess(name);
}

void Launcher::RestartProcess(QString name) {
    emit SIGNAL_Forward_RestartProcess(name);
}

void Launcher::run() {
    worker_ = new Worker(config_);

    // forward to the worker a queued wonnection
    QObject::connect(this, &Launcher::SIGNAL_Forward_GetProcesses, worker_, &Worker::GetProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_GetRealProcesses, worker_, &Worker::GetRealProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_GetRealRequiredProcesses, worker_, &Worker::GetRealRequiredProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StartRealProcesses, worker_, &Worker::StartRealProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StartRealRequiredProcesses, worker_, &Worker::StartRealRequiredProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StartSimProcesses, worker_, &Worker::StartSimProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StopProcesses, worker_, &Worker::StopProcesses);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StartProcess, worker_, &Worker::StartProcess);
    QObject::connect(this, &Launcher::SIGNAL_Forward_StopProcess, worker_, &Worker::StopProcess);
    QObject::connect(this, &Launcher::SIGNAL_Forward_ToggleProcessState, worker_, &Worker::ToggleProcessState);
    QObject::connect(this, &Launcher::SIGNAL_Forward_RestartProcess, worker_, &Worker::RestartProcess);

    // from worker to public
    QObject::connect(worker_, &Worker::SIGNAL_Error, this, &Launcher::SIGNAL_Error);
    QObject::connect(worker_, &Worker::SIGNAL_ProcessCreated, this, &Launcher::SIGNAL_ProcessCreated);
    QObject::connect(worker_, &Worker::SIGNAL_ProcessDeleted, this, &Launcher::SIGNAL_ProcessDeleted);
    QObject::connect(worker_, &Worker::SIGNAL_ProcessStarted, this, &Launcher::SIGNAL_ProcessStarted);
    QObject::connect(worker_, &Worker::SIGNAL_ProcessStopped, this, &Launcher::SIGNAL_ProcessStopped);
    QObject::connect(worker_, &Worker::SIGNAL_AllRequiredRunning, this, &Launcher::SIGNAL_AllRequiredRunning);
    QObject::connect(worker_, &Worker::SIGNAL_AllRequiredNotRunning, this, &Launcher::SIGNAL_AllRequiredNotRunning);

    exec();
}

} // namespace

