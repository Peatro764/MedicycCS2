#ifndef OMRONSELECTIONDIALOG_H
#define OMRONSELECTIONDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMap>
#include <QString>

namespace Ui {
class OmronSelectionDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::main {

class OmronSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OmronSelectionDialog(QWidget *parent);
    ~OmronSelectionDialog();
    QStringList Chosen();

public slots:

private slots:

private:
    Ui::OmronSelectionDialog *ui_;
};

}

#endif
