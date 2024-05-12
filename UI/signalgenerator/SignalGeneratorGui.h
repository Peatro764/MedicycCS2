#ifndef SIGNALGENERATORGUI_H
#define SIGNALGENERATORGUI_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QGridLayout>
#include <QMap>

#include "SignalGenerator.h"

namespace Ui {
class SignalGeneratorGui;
}

namespace hw_signalgenerator = medicyc::cyclotroncontrolsystem::hardware::signalgenerator;

class SignalGeneratorGui : public QWidget
{
    Q_OBJECT

public:
    explicit SignalGeneratorGui(QWidget *parent = 0);
    ~SignalGeneratorGui();

public slots:

private slots:
    void ShowPing();
    void ShowConnected();
    void ShowDisconnected();
    void SetAbsoluteAmplitude();
    void SetAbsoluteFrequency();
    void SetStepFrequency();
    void SetAddress();
    void ShowAddress(int address);

private:
    QString FrameStyleSheet(QString name, QString image) const;
    Ui::SignalGeneratorGui *ui_;
    hw_signalgenerator::SignalGenerator signal_generator_;
};

#endif
