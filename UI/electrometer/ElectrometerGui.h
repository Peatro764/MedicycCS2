#ifndef ELECTROMETERGUI_H
#define ELECTROMETERGUI_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>
#include <QMap>
#include <QMessageBox>

#include "electrometer_interface.h"
#include "SlidingGraph.h"

namespace Ui {
class ElectrometerGui;
}

namespace medicyc::cyclotroncontrolsystem::ui::electrometer {

class ElectrometerGui : public QWidget
{
    Q_OBJECT

public:
    explicit ElectrometerGui(QWidget *parent = 0, QString name = "Stripper");
    ~ElectrometerGui();

public slots:

private slots:
    void ShowPing();
    void ShowConnected();
    void ShowDisconnected();
    void ShowMeasurement(double timestamp, double value, bool ol);
    void PlotMeasurement(double timestamp, double value, bool ol);
    void ShowCurrentRange(double value);
    void ShowZero(double value);
    void ShowMean(double value);
    void ShowStdDev(double value);
    void ShowLeakageCurrentSelection(bool activated);
    void UserInfoPopup(QString message);
    bool UserQuestionPopup(QString question);
    void UserErrorPopup(QString message);

private:
    QString FrameStyleSheet(QString name, QString image) const;
    int GetNumberOfDigits(double value) const;
    Ui::ElectrometerGui *ui_;
    std::unique_ptr<QSettings> settings_;
    medicyc::cyclotron::ElectrometerInterface electrometer_interface_;
    QString unit_ = "NA";
    SlidingGraph* sliding_graph_;
};

}

#endif
