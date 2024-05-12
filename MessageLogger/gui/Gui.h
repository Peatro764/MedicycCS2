#ifndef MESSAGELOGGERGUI_H
#define MESSAGELOGGERGUI_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QPlainTextEdit>
#include <memory>

#include "messagelogger_interface.h"
#include <QtDBus/QDBusConnection>

namespace Ui {
class Gui;
}

using namespace medicyc::cyclotron;

class Gui : public QWidget
{
    Q_OBJECT

public:
    explicit Gui(QWidget *parent = 0);
    ~Gui();

public slots:

private slots:
    void DisplayError(qlonglong datetime, const QString &system, const QString &message);
    void DisplayWarning(qlonglong datetime, const QString &system, const QString &message);
    void DisplayInfo(qlonglong datetime, const QString &system, const QString &message);

private:
    void SetupConnection();
    void SetupLayout();
    void DisplayMessage(QString line, QColor color);
    QString GetFormattedMessage(qlonglong datetime, const QString& system, const QString& message) const;
    MessageLoggerInterface* iserver_;

    Ui::Gui *ui_;
    std::unique_ptr<QSettings> settings_;
    QPlainTextEdit *textfield_;

};

#endif
