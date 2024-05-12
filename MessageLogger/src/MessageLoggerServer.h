#ifndef MEDICYCCS2_MESSAGELOGGERSERVER_H
#define MEDICYCCS2_MESSAGELOGGERSERVER_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QState>
#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDateTime>

namespace medicyc::cyclotroncontrolsystem::messagelogger {

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
        Worker(DatabaseConfig config, QObject* parent = 0);
        ~Worker();

    public:
        void Connect();
        void SaveError(qint64 datetime, QString system, QString message);
        void SaveWarning(qint64 datetime, QString system, QString message);
        void SaveInfo(qint64 datetime, QString system, QString message);
        void SaveDebug(qint64 datetime, QString system, QString message);

        void GetMessages(qint64 from, qint64 to);

    signals:
        void SIGNAL_Error(qint64 datetime, QString system, QString message);
        void SIGNAL_Warning(qint64 datetime, QString system, QString message);
        void SIGNAL_Info(qint64 datetime, QString system, QString message);
        void SIGNAL_Debug(qint64 datetime, QString system, QString message);
        void SIGNAL_RetrievedMessages(qint64 datetime, QString system, QString type, QString content);

    private:
        void InternalErrorMessage(QString message);
        void SaveMessage(QDateTime timestamp, QString system, QString type, QString content);
        DatabaseConfig config_;
        bool error_active_ = false;
};


class MessageLoggerServer : public QThread {
    Q_OBJECT
public:
    MessageLoggerServer(QString db_suffix);
    ~MessageLoggerServer();

public slots:
    void Error(qint64 datetime, QString system, QString message);
    void Warning(qint64 datetime, QString system, QString message);
    void Info(qint64 datetime, QString system, QString message);
    void Debug(qint64 datetime, QString system, QString message);

    void GetMessages(qint64 from, qint64 to);

private slots:

signals: // public
    void SIGNAL_Error(qint64 datetime, QString system, QString message);
    void SIGNAL_Warning(qint64 datetime, QString system, QString message);
    void SIGNAL_Info(qint64 datetime, QString system, QString message);
    void SIGNAL_Debug(qint64 datetime, QString system, QString message);
    void SIGNAL_RetrievedMessages(qint64 datetime, QString system, QString type, QString content);

signals: // private
    void SIGNAL_GetMessages(qint64 from, qint64 to);

private:
    void run() override;
    const QString DB_BASE_NAME_ = "timeseriesdatabase";
    QString db_name_;
    DatabaseConfig config_;
    Worker* worker_ = nullptr;
};

}

#endif
