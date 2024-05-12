#ifndef MESSAGEMODEL_H
#define MESSAGEMODEL_H

#include <QVector>
#include <QDateTime>
#include <QAbstractTableModel>

#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class MessageModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    MessageModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    medicyc::cyclotron::MessageLoggerInterface messagelogger_interface_;

    QVector<QDateTime> timestamps_;
    QVector<QString> type_;
    QVector<QString> system_;
    QVector<QString> message_;
    const int MAX_MESSAGES = 1000;
    bool show_error_ = true;
    bool show_warning_ = true;
    bool show_info_ = true;
    bool show_debug_ = false;

};

}

#endif // MESSAGEMODEL_H
