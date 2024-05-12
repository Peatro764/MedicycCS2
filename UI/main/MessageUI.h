#ifndef MESSAGEUI_H
#define MESSAGEUI_H

#include <QWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QRadioButton>

#include "messagelogger_interface.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class MessageUI : public QWidget
{
    Q_OBJECT
public:
    MessageUI(QWidget *parent);
    ~MessageUI();

public slots:
    void DebugMessage(QString text);
    void InfoMessage(QString text);
    void WarningMessage(QString text);
    void ErrorMessage(QString text);

signals:
    void SIGNAL_PlayErrorSound();

private slots:
    QHBoxLayout* CreateTopWindow();
    QHBoxLayout* CreateTypeSelector();
    QHBoxLayout* CreateDateSelector();
    QVBoxLayout* CreateMessageWindow();
    void DisplayMessage(QString type, qlonglong datetime, QString system, QString message, QColor color);
    void GetData(QDate from, QDate to);
    QRadioButton* GetRadioButton(QString text);

private:
    void ConfigureTableWidget();
    medicyc::cyclotron::MessageLoggerInterface messagelogger_interface_;
    int active_row_ = 0;
    const int MAX_MESSAGES_ = 1500;
    QTableWidget table_widget_;
    enum class COLUMNS { DATE = 0, TYPE = 1, SYSTEM = 2, MESSAGE = 3 };
    QVector<QString> blocked_systems_; // = { "NSingle Bobine_de_Correction_C04", "NSingle Bobine_de_Correction_C05"};
    bool show_error_ = true;
    bool show_warning_ = true;
    bool show_info_ = true;
    bool show_debug_ = false;
    bool update_auto_ = true;
};

}

#endif
