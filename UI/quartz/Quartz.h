#ifndef QUARTZ_H
#define QUARTZ_H

#include <QWidget>
#include <QSettings>
#include <QStandardPaths>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QTimer>
#include <QMessageBox>
#include <QThread>
#include <QStateMachine>
#include <QMap>

#include "VideoWorker.h"
#include "BeamPlot.h"
#include "QuartzRepo.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class Quartz : public QWidget
{
    Q_OBJECT

public:
    explicit Quartz();
    ~Quartz();

public slots:

private slots:
    void SetActiveQuartz(QString name);

    void ProcessImage(cv::Mat frame);
    cv::Mat IntegrateImage(cv::Mat frame);
    void SubtractImage(bool on);

    void SetIntegratedMode(bool on);
    void ClearIntegratedImage();

    void SetDefaultAxesCalibration();
    void ApplyAxesCalibration();
    void SaveAxesCalibration();
    void ImageSetGrid(bool on);
    void ImageSetContour(bool on);

    void SaveCurrentImage();
    void RetrieveCurrentImage();
    void ClearSavedImages();
    void SaveImageSet();
    void GetImageSet();

    void DisplayError(QString message);
    void DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon);
    void UpdateMirDiameter(int value);

signals:
    void SIGNAL_ContourMode(bool on);
    void SIGNAL_MaxPosition(QPointF pos);
    void SIGNAL_LeftCalibValue(QPointF pos);
    void SIGNAL_TopCalibValue(QPointF pos);
    void SIGNAL_RightCalibValue(QPointF pos);
    void SIGNAL_BottomCalibValue(QPointF pos);

    void SIGNAL_StartCalibration();
    void SIGNAL_CalibrationStarted();
    void SIGNAL_CalibrationStopped();

    void SIGNAL_QuartzSelected(QString name);
    void SIGNAL_ImageSetGridMode(bool on);
    void SIGNAL_ImageSetContourMode(bool on);

private:
    void SetupUI();
    void ConnectSignals();
    void SetupVideoFeed();
    void SetupCalibrationStateMachine();
    QFrame* SetupCommandFrame();
    QFrame* SetupImageSetCommandFrame();
//    QFrame* SetupResultsFrame();
    QFrame* SetupQuartzSelectionFrame();
    QFrame* SetupSavedImagesFrame();
    void SetStandardStyleSheet(QWidget* widget);
    QSettings settings_;
    QuartzRepo repo_;
    QThread video_thread_;
    VideoWorker* video_worker_;

    std::vector<QString> quartzes {"QZ1", "QZ2", "QZ3", "QZ4", "QZ5", "QZ6", "QZ7", "QZ8", "QZ9", "QZ10", "QZ11", "QZ12"};

    BeamPlot beam_plot_;
    QMap<QString, BeamPlot*> saved_beam_plots_;

    cv::Mat processed_image_;
    cv::Mat subtracted_image_;
    cv::Mat integrated_image_;
    int n_integrations_ = 0;
    bool signal_mode_integrate_ = false;
    bool show_stream_ = true;

    struct {
        QPointF left;
        QPointF top;
        QPointF right;
        QPointF bottom;
    } calib_pos_ ;
    QPointF mouse_click_pos_;
    QStateMachine sm_calibration_;

    float mir_diameter_ = 30.0;
    const float X_PIXELS = 720 / 1.1; // real quartz
    const float Y_PIXELS = 576 / 1.1; // real quartz
//    const float X_PIXELS = 1280; // camera
//    const float Y_PIXELS = 720; // camera

    const int MAXNCOL = 4;
};

}

#endif
