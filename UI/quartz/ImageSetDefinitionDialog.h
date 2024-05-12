#ifndef IMAGESETDEFINITIONDIALOG_H
#define IMAGESETDEFINITIONDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMap>
#include <QString>

namespace Ui {
class ImageSetDefinitionDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class ImageSetDefinitionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSetDefinitionDialog(QWidget *parent);
    ~ImageSetDefinitionDialog();
    QString identifier() const;
    QString comments() const;

public slots:

private slots:

private:
    void ConnectSignals();
    Ui::ImageSetDefinitionDialog *ui_;
};

}

#endif
