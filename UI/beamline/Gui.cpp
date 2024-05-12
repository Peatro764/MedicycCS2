#include "Gui.h"

#include <QPainterPath>
#include <QGraphicsView>
#include <QOpenGLWidget>
#include <QSlider>
#include <QRadioButton>
#include <QPlainTextEdit>
#include <QDateTime>
#include <qscrollbar.h>

#include "Parameters.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

Gui::Gui()
    : settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                QSettings::IniFormat)
{
    qDebug() << "Gui::Gui";
    setWindowTitle("Voie de faisceau");
    setFixedSize(1280, 1050); // 1440
    setAutoFillBackground(true);
    scene_ = new Scene(this);
    view_ = new QGraphicsView(scene_);
    view_->verticalScrollBar()->hide();
    view_->horizontalScrollBar()->hide();


    QOpenGLWidget *gl = new QOpenGLWidget();
    QSurfaceFormat format;
    format.setSamples(4);
    gl->setFormat(format);
    view_->setViewport(gl);

    scene_->setSceneRect(0, 0, 800, 800);
    view_->setSceneRect(0, 0, 800, 800);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(view_, 0, 0);
    this->setLayout(gridLayout);

    //    /* Let's color it a little to better realize the positioning: */
    //        setStyleSheet("QWidget {"
    //                      "border: 1px solid black;"
    //                      "color: red"
    //                      "}");    
}

Gui::~Gui()
{
    delete scene_;
    delete view_;
    qDebug() << "Gui::~Gui";
}

} // ns
