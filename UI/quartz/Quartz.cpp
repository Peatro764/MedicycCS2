#include "Quartz.h"

#include <QStandardPaths>
#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSizePolicy>
#include <QSpinBox>
#include <QGraphicsView>
#include <QRgb>
#include <qwt_scale_map.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <QSignalTransition>

#include "ImageSetSelectionDialog.h"
#include "ImageSetDefinitionDialog.h"
#include "Utils.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::ui::quartz {

Quartz::Quartz()
    : settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                QSettings::IniFormat),
      repo_("_prod"),
      beam_plot_("", this)
{
    setWindowTitle("QUARTZ");
    setFixedSize(1100, 1440);
    SetupCalibrationStateMachine();
    ConnectSignals();
    SetupUI();
    SetupVideoFeed();
    //    /* Let's color it a little to better realize the positioning: */
//    setStyleSheet("QWidget {"
//                  "border: 1px solid black;"
//                  "color: red"
//                  "}");
    SetActiveQuartz(quartzes.at(0));
    SetStandardStyleSheet(this);
}

Quartz::~Quartz()
{
    if (video_worker_) delete video_worker_;
}

void Quartz::ConnectSignals() {
    QObject::connect(this, &Quartz::SIGNAL_QuartzSelected, this, &Quartz::SetActiveQuartz);
    QObject::connect(this, &Quartz::SIGNAL_ImageSetContourMode, this, &Quartz::ImageSetContour);
    QObject::connect(this, &Quartz::SIGNAL_ImageSetGridMode, this, &Quartz::ImageSetGrid);
}

void Quartz::SetActiveQuartz(QString name) {
    qDebug() << "Quartz::SetActiveQuartz " << name;
    beam_plot_.SetName(name);
    SetDefaultAxesCalibration();
}

void Quartz::SetupUI() {
    qDebug() << "Quartz::SetupUI";
    this->setObjectName("QuartzGui");

    // Setup central Area
    beam_plot_.setMinimumWidth(X_PIXELS + 115);
    beam_plot_.setMaximumWidth(X_PIXELS + 115);
    beam_plot_.setMinimumHeight(Y_PIXELS + 30);
    beam_plot_.setMaximumHeight(Y_PIXELS + 30);

    QFrame *commandFrame = SetupCommandFrame();
    QFrame *quartzSelectionFrame = SetupQuartzSelectionFrame();
    QFrame *savedImagesFrame = SetupSavedImagesFrame();
    QFrame *imageSeriesCommandFrame = SetupImageSetCommandFrame();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(commandFrame);
    layout->addWidget(&beam_plot_);

    // Put everything on the grid layout
    QGridLayout *gridLayout = new QGridLayout;
//    gridLayout->setSpacing(0);
    gridLayout->addLayout(layout, 0, 0);
    gridLayout->addWidget(quartzSelectionFrame, 1, 0, 1, 1);
    gridLayout->addWidget(savedImagesFrame, 2, 0, 1, 1);
    gridLayout->addWidget(imageSeriesCommandFrame, 3, 0, 1, 1);

    this->setLayout(gridLayout);    
}

void Quartz::SetupVideoFeed() {
    qDebug() << "Quartz::SetupVideoFeed";
    video_worker_ = new VideoWorker,
    video_worker_->moveToThread(&video_thread_);
    QObject::connect(&video_thread_, &QThread::finished, &video_thread_, &QObject::deleteLater);
    QObject::connect(video_worker_, &VideoWorker::SIGNAL_VideoFeedRead, this, &Quartz::ProcessImage);
    QObject::connect(video_worker_, &VideoWorker::SIGNAL_Error, this, &Quartz::DisplayError);
    QTimer::singleShot(1000, video_worker_, &VideoWorker::SetupStream);
    video_thread_.start();
}

void Quartz::SetDefaultAxesCalibration() {
    qDebug() << "Quartz::SetDefaultAxesCalibration";
    try {
        double x_min, x_max, y_min, y_max;
        repo_.GetBeamAxesCalibration(beam_plot_.GetName(), x_min, x_max, y_min, y_max);
        beam_plot_.SetInterval(QwtInterval(x_min, x_max), QwtInterval(y_min, y_max));
    } catch (std::exception& exc) {
        qWarning() << "Quartz::SetDefaultAxesCalibration Exception thrown: " << exc.what();
    }
}

void Quartz::ProcessImage(cv::Mat frame) {
//    qDebug() << "Quartz::ProcessImage cv::mat Depth" << frame.depth() << " Col " << frame.cols << " Row " << frame.rows
//             << " Channels " << frame.channels() << " Type " << frame.type();
    if (show_stream_) {
        cv::Mat flipped;
        cv::flip(frame, flipped, 0); // flip around horisontal axis

        cv::Mat greyFrame;
        cv::cvtColor(flipped, greyFrame, cv::COLOR_BGR2GRAY);

        processed_image_ = (signal_mode_integrate_ ? IntegrateImage(greyFrame) : greyFrame);
        if (subtracted_image_.empty()) {
            beam_plot_.SetImage(processed_image_);
        } else {
            cv::Mat subtracted_frame = processed_image_ - subtracted_image_;
            beam_plot_.SetImage(subtracted_frame);
        }
        emit SIGNAL_MaxPosition(beam_plot_.GetMaxPosition());
    }
}

void Quartz::SubtractImage(bool on) {
    qDebug() << "Quartz::SubtractImage " << on;
    if (on) {
        subtracted_image_ = processed_image_;
    } else {
        subtracted_image_ = cv::Mat();
    }
}

cv::Mat Quartz::IntegrateImage(cv::Mat frame) {
    cv::Mat floatFrame;
    frame.convertTo(floatFrame, CV_32F);
    n_integrations_++;
    if (integrated_image_.empty()) {
        integrated_image_ = floatFrame;
    } else {
        integrated_image_ = ((static_cast<double>(n_integrations_ - 1)/ static_cast<double>(n_integrations_)))*integrated_image_ + floatFrame * (1.0 / static_cast<double>(n_integrations_));
    }
    cv::Mat intFrame;
    integrated_image_.convertTo(intFrame, CV_8UC1);
    return intFrame;
}

void Quartz::SetIntegratedMode(bool on) {
    qDebug() << "Quartz::SetIntegratedMode " << on;
    signal_mode_integrate_ = on;
    if (!on) ClearIntegratedImage();
}

void Quartz::ClearIntegratedImage() {
    qDebug() << "Quartz::ClearIntegratedImage";
    n_integrations_ = 0;
    integrated_image_ = cv::Mat();
}

void Quartz::DisplayError(QString message) {
    DisplayModelessMessageBox(message, true, 4, QMessageBox::Critical);
}

void Quartz::DisplayModelessMessageBox(QString msg, bool auto_close, int sec, QMessageBox::Icon icon) {
    QMessageBox* box = new QMessageBox(this);
    box->setText(msg);
    box->setIcon(icon);
    box->setModal(false);
    box->setAttribute(Qt::WA_DeleteOnClose);
    if (auto_close) QTimer::singleShot(sec * 1000, box, SLOT(close()));
    box->show();
}

void Quartz::UpdateMirDiameter(int value) {
    mir_diameter_ = static_cast<double>(value);
}

QFrame* Quartz::SetupQuartzSelectionFrame() {
    qDebug() << "Quartz::SetupQuartzSelectionFrame";
    QFrame *frame = new QFrame;
    frame->setObjectName("QuartzSelectionName");
    frame->setAttribute(Qt::WA_TranslucentBackground);
    QGridLayout *layout = new QGridLayout;
    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive(true);
    int iButton = 0;
    int row = 0;
    int col = 0;
    for(const auto& q: quartzes) {
        QPushButton *b = new QPushButton(q);
        b->setMinimumHeight(40);
        b->setCheckable(true);
        layout->addWidget(b, row, col);
        if (col < (MAXNCOL - 1)) {
            col++;
        } else {
            row++;
            col = 0;
        }
        buttonGroup->addButton(b, iButton++);
        QObject::connect(b, &QPushButton::clicked, this, [&,q]() { emit SIGNAL_QuartzSelected(q); });
    }
    buttonGroup->button(0)->setChecked(true);
    frame->setLayout(layout);
    return frame;
}

void Quartz::SetStandardStyleSheet(QWidget* widget) {
    QString stylesheet;
    stylesheet = "QWidget#" + widget->objectName() + "{" +
            "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 black, stop: 1 cornflowerblue);" +
               + "border-radius: 7px;" + "}";
    widget->setStyleSheet(stylesheet);
}

QFrame* Quartz::SetupCommandFrame() {
    qDebug() << "Quartz::SetupCommandFrame";
    QFrame *frame = new QFrame;
    frame->setMaximumWidth(300);
    frame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    frame->setObjectName("leftSideFrame");
    frame->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->setSpacing(0);
    frame->setLayout(sideLayout);

    QFont font1("Arial", 13);
    font1.setBold(false);

    // read video
    QPushButton *b_read_video = new QPushButton("Image stream");
    b_read_video->setFont(font1);
    b_read_video->setMinimumHeight(40);
    b_read_video->setCheckable(true);
    b_read_video->setChecked(true);
    QObject::connect(b_read_video , &QPushButton::clicked, this, [&](bool checked){ show_stream_ = checked; });

    // Signal mode (integration or instantenous)
    QPushButton *b_integration = new QPushButton("Intégration signal");
    b_integration->setFont(font1);
    b_integration->setMinimumHeight(40);
    b_integration->setChecked(false);
    b_integration->setCheckable(true);
    QObject::connect(b_integration , &QPushButton::clicked, this, &Quartz::SetIntegratedMode);

    // Subtract image (on/off)
    QPushButton *b_subtract = new QPushButton("Soustraire signal");
    b_subtract->setFont(font1);
    b_subtract->setMinimumHeight(40);
    b_subtract->setChecked(false);
    b_subtract->setCheckable(true);
    QObject::connect(b_subtract, &QPushButton::clicked, this, &Quartz::SubtractImage);
    QObject::connect(b_subtract, &QPushButton::clicked, this, [&](bool checked) { if (checked) ClearIntegratedImage(); });

    // Contour mode (on/off)
    QPushButton *b_contour = new QPushButton("Contours");
    b_contour->setFont(font1);
    b_contour->setMinimumHeight(40);
    b_contour->setChecked(false);
    b_contour->setCheckable(true);
    QObject::connect(b_contour, &QPushButton::clicked, this, [&](bool checked){ beam_plot_.ShowContour(checked); });

    // Grid mode (on/off)
    QPushButton *b_grid = new QPushButton("Grid");
    b_grid->setFont(font1);
    b_grid->setMinimumHeight(40);
    b_grid->setChecked(false);
    b_grid->setCheckable(true);
    QObject::connect(b_grid, &QPushButton::clicked, this, [&](bool checked){ beam_plot_.DrawGrid(checked); });

    // Calibrate
    QHBoxLayout *calibrationLayout = new QHBoxLayout;

    QPushButton *b_calibrate = new QPushButton("Etalonner axes");
    b_calibrate->setCheckable(true);
    b_calibrate->setChecked(true);
    b_calibrate->setMinimumHeight(40);
    b_calibrate->setFont(font1);
    QObject::connect(b_calibrate, &QPushButton::clicked, this, &Quartz::SIGNAL_StartCalibration);
    QObject::connect(this, &Quartz::SIGNAL_CalibrationStarted, [&, b_calibrate]() { b_calibrate->setChecked(true); });
    QObject::connect(this, &Quartz::SIGNAL_CalibrationStopped, [&, b_calibrate]() { b_calibrate->setChecked(false); });
    calibrationLayout->addWidget(b_calibrate);

    QSpinBox *spinBox_mirdiameter = new QSpinBox;
    spinBox_mirdiameter->setAlignment(Qt::AlignCenter);
    spinBox_mirdiameter->setMinimumHeight(40);
    spinBox_mirdiameter->setMaximumWidth(40);
    spinBox_mirdiameter->setValue(mir_diameter_);
    spinBox_mirdiameter->setMinimum(1);
    spinBox_mirdiameter->setMaximum(200);
    calibrationLayout->setSpacing(10);
    calibrationLayout->addWidget(spinBox_mirdiameter);
    QObject::connect(spinBox_mirdiameter, qOverload<int>(&QSpinBox::valueChanged), this, &Quartz::UpdateMirDiameter);

    QPushButton *b_save_calibration = new QPushButton("Sauvegarder\nétannolage");
    b_save_calibration->setMinimumHeight(40);
    b_save_calibration->setFont(font1);
    QObject::connect(b_save_calibration, &QPushButton::clicked, this, &Quartz::SaveAxesCalibration);

    QPushButton *b_transfer_image = new QPushButton("Transférer\nImage");
    b_transfer_image->setMinimumHeight(40);
    b_transfer_image->setFont(font1);
    QObject::connect(b_transfer_image, &QPushButton::clicked, this, &Quartz::SaveCurrentImage);

    QPushButton *b_recuperer_image = new QPushButton("Récuperer\nImage");
    b_recuperer_image->setMinimumHeight(40);
    b_recuperer_image->setFont(font1);
    QObject::connect(b_recuperer_image, &QPushButton::clicked, this, &Quartz::RetrieveCurrentImage);

    // Put everything together
    int spacing = 0;
    sideLayout->addWidget(b_read_video, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_integration, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_subtract, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_contour, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_grid, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addLayout(calibrationLayout, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_save_calibration, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_transfer_image, Qt::AlignTop);
    sideLayout->addSpacing(spacing);
    sideLayout->addWidget(b_recuperer_image, Qt::AlignTop);
    return frame;
}

QFrame* Quartz::SetupImageSetCommandFrame() {
    qDebug() << "Quartz::SetupImageSetCommandFrame";
    QFrame *frame = new QFrame;
    frame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    frame->setObjectName("bottomFrame");
    frame->setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    frame->setLayout(layout);

    QFont font1("Arial", 14);
    font1.setBold(false);

    // Contour mode (on/off)
    QPushButton *b_contour = new QPushButton("Contours");
    b_contour->setMinimumHeight(40);
    b_contour->setFont(font1);
    b_contour->setChecked(false);
    b_contour->setCheckable(true);
    QObject::connect(b_contour, &QPushButton::clicked, this, &Quartz::SIGNAL_ImageSetContourMode);

    // Grid mode (on/off)
    QPushButton *b_grid = new QPushButton("Grid");
    b_grid->setMinimumHeight(40);
    b_grid->setFont(font1);
    b_grid->setChecked(false);
    b_grid->setCheckable(true);
    QObject::connect(b_grid, &QPushButton::clicked, this, &Quartz::SIGNAL_ImageSetGridMode);

    QPushButton *b_save = new QPushButton("Sauvegarder");
    b_save->setMinimumHeight(40);
    b_save->setFont(font1);
    QObject::connect(b_save, &QPushButton::clicked, this, &Quartz::SaveImageSet);

    QPushButton *b_get = new QPushButton("Récuperer");
    b_get->setMinimumHeight(40);
    b_get->setFont(font1);
    QObject::connect(b_get, &QPushButton::clicked, this, &Quartz::GetImageSet);

    // Put everything together
    layout->addWidget(b_contour);
    layout->addWidget(b_grid);
    layout->addWidget(b_save);
    layout->addWidget(b_get);
    return frame;
}

void Quartz::ImageSetContour(bool on) {
    for (QString &q : quartzes) {
        if (saved_beam_plots_.contains(q)) {
            BeamPlot *p = saved_beam_plots_[q];
            p->ShowContour(on);
        }
    }
}

void Quartz::ImageSetGrid(bool on) {
    for (QString &q : quartzes) {
        if (saved_beam_plots_.contains(q)) {
            BeamPlot *p = saved_beam_plots_[q];
            p->DrawGrid(on);
        }
    }
}

void Quartz::SaveCurrentImage() {
    qDebug() << "Quartz::SaveCurrentImage";
    cv::Mat frame = beam_plot_.GetImage();
    QString active_quartz = beam_plot_.GetName();
    if (saved_beam_plots_.contains(active_quartz)) {
        QwtInterval x_int, y_int;
        beam_plot_.GetInterval(x_int, y_int);
        saved_beam_plots_[active_quartz]->SetInterval(x_int, y_int);
        saved_beam_plots_[active_quartz]->SetImage(frame);
        saved_beam_plots_[active_quartz]->ApplyZoom(beam_plot_.GetZoom());
    } else {
        qDebug() << "Quartz::SaveCurrentImage quarts does not exist in vector " << active_quartz << " " << saved_beam_plots_.keys();
    }
}

void Quartz::RetrieveCurrentImage() {
    qDebug() << "Quartz::RetrieveCurrentImage";
    QString active_quartz = beam_plot_.GetName();
    if (saved_beam_plots_.contains(active_quartz)) {
        QwtInterval x_int, y_int;
        BeamPlot *beam_plot = saved_beam_plots_[active_quartz];
        QVector<double> data;
        int ncol;
        beam_plot->GetData(data, ncol);
        beam_plot->GetInterval(x_int, y_int);
        beam_plot_.SetInterval(x_int, y_int);
        beam_plot_.SetData(data, ncol);
        beam_plot_.ApplyZoom(beam_plot->GetZoom());
    } else {
        qDebug() << "Quartz::SaveCurrentImage quarts does not exist in vector " << active_quartz << " " << saved_beam_plots_.keys();
    }
}

void Quartz::SaveAxesCalibration() {
    qDebug() << "Quartz::SaveAxesCalibration";
    QwtInterval x_int, y_int;
    beam_plot_.GetInterval(x_int, y_int);
    try {
        repo_.SetBeamAxesCalibration(beam_plot_.GetName(), x_int.minValue(), x_int.maxValue(), y_int.minValue(), y_int.maxValue());
    } catch (std::exception& exc) {
        qWarning() << "Quartz::SaveAxesCalibration Exception caught " << exc.what();
        DisplayError("Failed saving calibration to db");
    }
}

void Quartz::ClearSavedImages() {
    qDebug() << "Quartz::ClearSavedImages";
    for(QString& q : quartzes) {
        if (saved_beam_plots_.contains(q)) {
            BeamPlot *p = saved_beam_plots_[q];
            QVector<double> data;
            int ncol;
            p->GetData(data, ncol);
            if (data.size()) {
                std::fill(data.begin(), data.end(), 0.0);
                p->SetData(data, ncol);
            }
        }
    }
}

QFrame* Quartz::SetupSavedImagesFrame() {
    qDebug() << "Quartz::SetupSavedImagesFrame";
    QFrame *frame = new QFrame;
    frame->setObjectName("SavedImagesFrame");
    frame->setAttribute(Qt::WA_TranslucentBackground);
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setSpacing(0);
    int row = 0;
    int col = 0;
    for(const auto& q: quartzes) {
        BeamPlot* beam_plot = new BeamPlot(q, this);
        beam_plot->setMinimumHeight(Y_PIXELS / 2.8);
        beam_plot->setMaximumHeight(Y_PIXELS / 2.8);
        beam_plot->setMinimumWidth(X_PIXELS / 2.8);
        beam_plot->setMaximumWidth(X_PIXELS / 2.8);
        beam_plot->enableAxis(QwtPlot::yRight, false);
        gridLayout->addWidget(beam_plot, row, col);
        saved_beam_plots_[q] = beam_plot;
        if (col < (MAXNCOL - 1)) {
            col++;
        } else {
            col = 0;
            row++;
        }
    }
    frame->setLayout(gridLayout);
    return frame;
}


void Quartz::SetupCalibrationStateMachine() {
    qDebug() << "Quartz::SetupCalibrationStateMachine";
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        QState *sCalibrate = new QState(sSuperState);
            QState *sLeft = new QState(sCalibrate);
            QState *sTop = new QState(sCalibrate);
            QState *sRight = new QState(sCalibrate);
            QState *sBottom = new QState(sCalibrate);
            QState *sApply = new QState(sCalibrate);

    QObject::connect(&beam_plot_, &BeamPlot::SIGNAL_MouseClick, this, [&](const QPointF& pos) { mouse_click_pos_ = pos; });

    // SuperState
    sSuperState->setInitialState(sIdle);

    // Idle
    sIdle->addTransition(this, &Quartz::SIGNAL_StartCalibration, sCalibrate);
    QObject::connect(sIdle, &QState::entered, this, [&](){ emit SIGNAL_CalibrationStopped(); });

    // Calibrate
    sCalibrate->setInitialState(sLeft);
    QObject::connect(sCalibrate, &QState::entered, this, [&](){ emit SIGNAL_CalibrationStarted(); });

        // Left
        QSignalTransition* tMouseClickInLeft = sLeft->addTransition(&beam_plot_, &BeamPlot::SIGNAL_MouseClick, sTop);
        QObject::connect(tMouseClickInLeft, &QSignalTransition::triggered, this, [&]() {
            calib_pos_.left = mouse_click_pos_;
            qDebug() << "LEFT " << mouse_click_pos_;
            emit SIGNAL_LeftCalibValue(mouse_click_pos_);
        });

        // Top
        QSignalTransition* tMouseClickInTop = sTop->addTransition(&beam_plot_, &BeamPlot::SIGNAL_MouseClick, sRight);
        QObject::connect(tMouseClickInTop, &QSignalTransition::triggered, this, [&]() {
            calib_pos_.top = mouse_click_pos_;
            emit SIGNAL_LeftCalibValue(mouse_click_pos_);
            qDebug() << "TOP " << mouse_click_pos_;
            emit SIGNAL_TopCalibValue(mouse_click_pos_);
        });

        // Right
        QSignalTransition* tMouseClickInRight = sRight->addTransition(&beam_plot_, &BeamPlot::SIGNAL_MouseClick, sBottom);
        QObject::connect(tMouseClickInRight, &QSignalTransition::triggered, this, [&]() {
            calib_pos_.right = mouse_click_pos_;
            qDebug() << "RIGHT " << mouse_click_pos_;
            emit SIGNAL_RightCalibValue(mouse_click_pos_);
        });

        // Bottom
        QSignalTransition* tMouseClickInBottom = sBottom->addTransition(&beam_plot_, &BeamPlot::SIGNAL_MouseClick, sApply);
        QObject::connect(tMouseClickInBottom, &QSignalTransition::triggered, this, [&]() {
            calib_pos_.bottom = mouse_click_pos_;
            qDebug() << "BOTTOM " << mouse_click_pos_;
            emit SIGNAL_BottomCalibValue(mouse_click_pos_);
        });

        // Apply
        QObject::connect(sApply, &QState::entered, this, &Quartz::ApplyAxesCalibration);
        sApply->addTransition(sApply, &QState::entered, sIdle);

    sm_calibration_.addState(sSuperState);
    sm_calibration_.setInitialState(sSuperState);
    sm_calibration_.start();
}

void Quartz::SaveImageSet() {
    qDebug() << "Quartz::SaveImageSet";
    try {
        ImageSetDefinitionDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted) {
            qDebug() << "Quartz::SaveImageSet Cancelled";
            return;
        }
        const QString image_set = dialog.identifier();
        const QString comments = dialog.comments();

        qDebug() << "Quartz::SaveImageSet Creating set " << image_set;
        repo_.CreateBeamImageSet(image_set, comments);

        for (QString& q : quartzes) {
            if (saved_beam_plots_.contains(q)) {
                qDebug() << "Quartz::SaveImageSet Saving image " << q;
                BeamPlot* image = saved_beam_plots_[q];
                QVector<double> data;
                int ncol;
                QwtInterval x_int, y_int;
                image->GetData(data, ncol);
                image->GetInterval(x_int, y_int);
                if (data.size() > 0) {
                repo_.SaveBeamImageData(image_set, q, data, ncol, x_int.minValue(), x_int.maxValue(),
                                        y_int.minValue(), y_int.maxValue(), image->GetZoom());
                } else {
                    qDebug() << "Quartz::SaveImageSet Skipping quartz since no data: " << q;
                }
            }
        }
        qDebug() << "Quartz::SaveImageSet " << dialog.comments() << " " << dialog.identifier();
    } catch(std::exception& exc) {
        qWarning() << "Quartz::SaveImageSet Exception caught " << exc.what();
        DisplayError("Failed saving images to db");
    }
    return;


}

void Quartz::GetImageSet() {
    try {
        qDebug() << "Quartz::GetImageSet";
        ClearSavedImages();
        QMap<QDateTime, QString> image_sets = repo_.GetBeamImageSets();
        ImageSetSelectionDialog dialog(this, image_sets);
        const int response = dialog.exec();
        if (response != QDialog::Accepted) {
            qDebug() << "Quartz::GetImageSet Cancelled";
            return;
        }
        auto image_set = dialog.selected_imageset();
        QStringList image_names = repo_.GetBeamImageNames(image_set);
        for (QString& image : image_names) {
            QVector<double> data;
            int ncol;
            double xmin, xmax, ymin, ymax;
            QRectF zoom;
            repo_.GetBeamImageData(image_set, image, data, ncol, xmin, xmax, ymin, ymax, zoom);
            if (saved_beam_plots_.contains(image)) {
                 saved_beam_plots_[image]->SetInterval(QwtInterval(xmin, xmax), QwtInterval(ymin, ymax));
                 saved_beam_plots_[image]->SetData(data, ncol);
                 saved_beam_plots_[image]->ApplyZoom(zoom);
            } else {
                qDebug() << "Quartz::GetImageSet Beam image not present " << image;
            }
        }
    } catch (std::exception& exc) {
        qWarning() << "Quartz::GetImageSet Exception caught " << exc.what();
        DisplayError("Failed retrieving images from db");
    }
}

void Quartz::ApplyAxesCalibration() {
    qDebug() << "Quartz::ApplyAxesCalibration";
    QwtScaleMap x_map = beam_plot_.canvasMap(QwtPlot::xBottom);
    double x_left_pixel = x_map.transform(calib_pos_.left.x());
    double x_right_pixel = x_map.transform(calib_pos_.right.x());
    double x_mm_per_pixel = mir_diameter_ / std::abs(x_left_pixel - x_right_pixel);
    double x_centre = x_mm_per_pixel * ((x_left_pixel + x_right_pixel) / 2.0 - X_PIXELS / 2.0);
    double x_min = -X_PIXELS * x_mm_per_pixel / 2.0 - x_centre;
    double x_max = X_PIXELS * x_mm_per_pixel / 2.0 - x_centre;
    qDebug() << "X mm per pixel " << x_mm_per_pixel;
    qDebug() <<  "X left pixel " << x_left_pixel << " X right pixel " << x_right_pixel << " X int" << x_min << " " << x_max << " Xcentre " << x_centre;

    QwtScaleMap y_map = beam_plot_.canvasMap(QwtAxis::YLeft);
    double y_top_pixel = y_map.transform(calib_pos_.top.y());
    double y_bottom_pixel = y_map.transform(calib_pos_.bottom.y());
    double y_mm_per_pixel = mir_diameter_ / std::abs(y_top_pixel - y_bottom_pixel);
    double y_centre = y_mm_per_pixel * ((y_top_pixel + y_bottom_pixel) / 2.0 - Y_PIXELS / 2.0);
    double y_min = -Y_PIXELS * y_mm_per_pixel / 2.0 + y_centre;
    double y_max = Y_PIXELS * y_mm_per_pixel / 2.0 + y_centre;
    qDebug() << "Y mm per pixel " << y_mm_per_pixel;
    qDebug() << "Y top pixel " << y_top_pixel << " Y bottom pixel " << y_bottom_pixel <<  "Y int" << y_min << " " << y_max << " Ycentre " << y_centre;

    beam_plot_.SetInterval(QwtInterval(x_min, x_max), QwtInterval(y_min, y_max));
}

} // ns
