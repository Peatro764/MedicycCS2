#ifndef GUI_H
#define GUI_H

#include <QWidget>
#include <QSettings>
#include <QStandardPaths>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

#include "Scene.h"

namespace medicyc::cyclotroncontrolsystem::ui::beamline {

class Gui : public QWidget
{
    Q_OBJECT

public:
    explicit Gui();
    ~Gui();

public slots:

private slots:

signals:

private:
    void SetupUI();
    QSettings settings_;
    Scene *scene_;
    QGraphicsView *view_;
};

}

#endif
