#ifndef CONFIGSAVEDIALOG_H
#define CONFIGSAVEDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMap>
#include <QString>

namespace Ui {
class ConfigSaveDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::main {

class ConfigSaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigSaveDialog(QWidget *parent);
    ~ConfigSaveDialog();
    QString name() const { return name_; }
    QString comments() const { return comments_; }

public slots:

private slots:
    void CheckNameValidity();

private:
    void ConnectSignals();
    Ui::ConfigSaveDialog *ui_;
    QString name_;
    QString comments_;
};

}

#endif
