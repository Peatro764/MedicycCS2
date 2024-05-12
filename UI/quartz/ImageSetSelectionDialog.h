#ifndef IMAGESETSELECTIONDIALOG_H
#define IMAGESETSELECTIONDIALOG_H

#include <QDialog>
#include <QDateTime>
#include <QMap>
#include <QString>

namespace Ui {
class ImageSetSelectionDialog;
}

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class ImageSetSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImageSetSelectionDialog(QWidget *parent, QMap<QDateTime, QString> available_sets);
    ~ImageSetSelectionDialog();
    QString selected_imageset() const { return selected_imageset_; }

public slots:

private slots:
    void GetSelectedImageSet();

private:
    void ConnectSignals();
    void SetupTable();
    void FillTable();
    void FillRow(int row, QDateTime timestamp, QString name, QString comments);
    enum class COLUMNS : int { TIMESTAMP = 0, NAME = 1, COMMENTS = 2 };
    Ui::ImageSetSelectionDialog *ui_;
    QMap<QDateTime, QString> available_imagesets_;
    QString selected_imageset_;
};

}

#endif
