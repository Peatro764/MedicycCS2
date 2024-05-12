#ifndef VideoWorker_H
#define VideoWorker_H

#include <QStateMachine>
#include <QTimer>
#include "opencv2/opencv.hpp"
#include <memory>
#include <QSettings>
#include <QImage>

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

class VideoWorker : public QObject {
  Q_OBJECT

   public:
    VideoWorker(QObject* parent = 0);
    ~VideoWorker();

signals:
    void SIGNAL_NoVideoFeed(int channel);
    void SIGNAL_VideoFeedOpen();
    void SIGNAL_VideoFeedClosed();
    void SIGNAL_VideoFeedRead(cv::Mat frame);
    void SIGNAL_Error(QString message);

public slots:
    void SetupStream();

private slots:
    void OpenVideoFeed();
    void CloseVideoFeed();
    void ReadVideoFeed();

private:
    void PrintStateChanges(QState *state, QString name);
    cv::VideoCapture video;
    int camera_channel_ = 0;//0;
    int read_delay_ = 100;
};

}

#endif // VideoWorker_H
