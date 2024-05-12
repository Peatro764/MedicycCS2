#ifndef NUMERICINPUTDIALOG_H
#define NUMERICINPUTDIALOG_H

#include <QDialog>

namespace Ui {
class NumericInputDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

class NumericInputDialog : public QDialog
{
    Q_OBJECT

public:
    NumericInputDialog(QWidget *parent);
    ~NumericInputDialog();

signals:
    void SIGNAL_Value(double value);

public slots:

private slots:
    void Validate();

private:
    void AddNumber(QString input);
    Ui::NumericInputDialog *ui_;
};

}

#endif

