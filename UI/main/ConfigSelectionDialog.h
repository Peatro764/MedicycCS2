#ifndef CONFIGSELECTIONDIALOG_H
#define CONFIGSELECTIONDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMap>
#include <QString>

namespace Ui {
class ConfigSelectionDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::main {

class ConfigSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigSelectionDialog(QWidget *parent, QMap<QDateTime, QStringList> available_configs);
    ~ConfigSelectionDialog();
    QString selected_config() const { return selected_config_; }

public slots:

private slots:
    void GetSelectedConfig();

private:
    void ConnectSignals();
    void SetupTable();
    void FillTable();
    void FillRow(int row, QDateTime timestamp, QString name, QString comments);
    enum class COLUMNS : int { TIMESTAMP = 0, NAME = 1, COMMENTS = 2 };
    Ui::ConfigSelectionDialog *ui_;
    QMap<QDateTime, QStringList> available_configs_;
    QString selected_config_;
};

}

#endif
