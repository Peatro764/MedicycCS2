#include "MessageLoggerServer.h"

#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QStandardPaths>
#include <QSqlError>

#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::messagelogger {

Worker::Worker(DatabaseConfig config, QObject* parent)
    : QObject(parent), config_(config) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL7", "WorkerThread");
    db.setHostName(config_.host_name);
    db.setPort(config_.port);
    db.setDatabaseName(config_.database_name);
    db.setUserName(config_.username);
    db.setPassword(config_.password);
    db.setConnectOptions(QString("connect_timeout=") + QString::number(config_.connection_timeout));
}

Worker::~Worker() {
    QSqlDatabase::removeDatabase("WorkerThread");
}

void Worker::Connect() {
    QSqlDatabase repo = QSqlDatabase::database("WorkerThread");
    if (repo.open()) {
        emit SIGNAL_Info(QDateTime::currentDateTimeUtc().toMSecsSinceEpoch(), "MessageLogger", "Successfully opened database");
    } else {
        InternalErrorMessage("Failed open database");
    }
}

void Worker::InternalErrorMessage(QString message) {
    if (!error_active_) {
        qDebug() << "Worker::ErrorMessage " << message;
        emit SIGNAL_Error(QDateTime::currentMSecsSinceEpoch(), "MessageLogger", message);
        error_active_ = true;
    }
}

void Worker::SaveError(qint64 datetime, QString system, QString content) {
    SaveMessage(QDateTime::fromMSecsSinceEpoch(datetime), system, "ERROR", content);
}

void Worker::SaveWarning(qint64 datetime, QString system, QString content) {
    SaveMessage(QDateTime::fromMSecsSinceEpoch(datetime), system, "WARNING", content);
}

void Worker::SaveInfo(qint64 datetime, QString system, QString content) {
    SaveMessage(QDateTime::fromMSecsSinceEpoch(datetime), system, "INFO", content);
}

void Worker::SaveDebug(qint64 datetime, QString system, QString content) {
    SaveMessage(QDateTime::fromMSecsSinceEpoch(datetime), system, "DEBUG", content);
}

void Worker::SaveMessage(QDateTime timestamp, QString system, QString type, QString content) {
    (void)timestamp;
    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "WorkerThread::SaveMessage Failed open";
        InternalErrorMessage("Database not open");
        return;
    }

    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("INSERT INTO Messages(time, system, type, content) "
                  "VALUES(:time, :system, :type, :content)");
    query.bindValue(":time", QDateTime::currentDateTimeUtc());
    query.bindValue(":system", system);
    query.bindValue(":type", type);
    query.bindValue(":content", content);

    if (query.exec()) {
        error_active_ = false;
    } else {
        db.close();
        qWarning() << QString("Worker::SaveMessage Query failed:") << query.lastQuery();
        InternalErrorMessage(QString("Database insert failed: ") + query.lastError().text());
    }
}

void Worker::GetMessages(qint64 from, qint64 to) {
    QSqlDatabase db = QSqlDatabase::database("WorkerThread");
    if (!db.open()) {
        qWarning() << "WorkerThread::GetMessages Failed open";
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM (SELECT time, system, type, content FROM Messages "
                  "WHERE time >= :start_time AND time <= :end_time "
                  "ORDER BY time DESC LIMIT 1000) AS subquery ORDER BY time ASC");
    query.bindValue(":start_time", QDateTime::fromMSecsSinceEpoch(from));
    query.bindValue(":end_time", QDateTime::fromMSecsSinceEpoch(to));

    if (query.exec()){
        error_active_ = false;
    } else {
        db.close();
        qWarning() << QString("Worker::GetMessage Query failed:") << query.lastQuery();
        InternalErrorMessage(QString("Database get failed: ") + query.lastError().text());
        return;
    }

    while (query.next()) {
        emit SIGNAL_RetrievedMessages(query.value(0).toDateTime().toMSecsSinceEpoch(),
                                      query.value(1).toString(),
                                      query.value(2).toString(),
                                      query.value(3).toString());
    }
}

MessageLoggerServer::MessageLoggerServer(QString db_suffix)
    : db_name_(DB_BASE_NAME_ + db_suffix) {
    QSettings settings(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                       QSettings::IniFormat);
    config_.host_name = settings.value(db_name_ + "/host", "unknown").toString();
    config_.database_name = settings.value(db_name_ + "/name", "unknown").toString();
    config_.port = settings.value(db_name_ + "/port", "0").toInt();
    config_.username = settings.value(db_name_ + "/user", "unknown").toString();
    config_.password = settings.value(db_name_ + "/password", "unknown").toString();

    qDebug() << "MessageLoggerServer::MessagerLoggerServer Host " << config_.host_name
             << " database " << config_.database_name
             << " port " << config_.port
             << " username " << config_.username
             << " password " << config_.password;

    this->start();
}

MessageLoggerServer::~MessageLoggerServer() {
    this->quit(); // stops thread
    while (this->isRunning()) {}
    if (worker_) delete worker_;
}

void MessageLoggerServer::Error(qint64 datetime, QString system, QString message) {
    emit SIGNAL_Error(datetime, system, message);
}

void MessageLoggerServer::Warning(qint64 datetime, QString system, QString message) {
    emit SIGNAL_Warning(datetime, system, message);
}

void MessageLoggerServer::Info(qint64 datetime, QString system, QString message) {
    emit SIGNAL_Info(datetime, system, message);
}

void MessageLoggerServer::Debug(qint64 datetime, QString system, QString message) {
    emit SIGNAL_Debug(datetime, system, message);
}

void MessageLoggerServer::GetMessages(qint64 from, qint64 to) {
    emit SIGNAL_GetMessages(from, to);
}

void MessageLoggerServer::run() {
  worker_ = new Worker(config_);

  // forward to the worker a 'queued connection'!
  QObject::connect(this, &MessageLoggerServer::SIGNAL_Error, worker_, &Worker::SaveError);
  QObject::connect(this, &MessageLoggerServer::SIGNAL_Warning, worker_, &Worker::SaveWarning);
  QObject::connect(this, &MessageLoggerServer::SIGNAL_Info, worker_, &Worker::SaveInfo);
  QObject::connect(this, &MessageLoggerServer::SIGNAL_Debug, worker_, &Worker::SaveDebug);
  QObject::connect(this, &MessageLoggerServer::SIGNAL_GetMessages, worker_, &Worker::GetMessages);

  // Results
  QObject::connect(worker_,  &Worker::SIGNAL_Debug, this, &MessageLoggerServer::SIGNAL_Debug);
  QObject::connect(worker_,  &Worker::SIGNAL_Info, this, &MessageLoggerServer::SIGNAL_Info);
  QObject::connect(worker_,  &Worker::SIGNAL_Warning, this, &MessageLoggerServer::SIGNAL_Warning);
  QObject::connect(worker_,  &Worker::SIGNAL_Error, this, &MessageLoggerServer::SIGNAL_Error);
  QObject::connect(worker_,  &Worker::SIGNAL_RetrievedMessages , this, &MessageLoggerServer::SIGNAL_RetrievedMessages);

  exec();  // start our own event loop
}

} // namespace
