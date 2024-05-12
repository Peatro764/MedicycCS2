#ifndef POLELEVELINDICATOR_H
#define POLELEVELINDICATOR_H

#include <QTime>
#include <QWidget>
#include <QColor>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT PoleLevelIndicator : public QWidget
{

public:
    explicit PoleLevelIndicator(QWidget *parent = 0, int width = 30, int height = 90,  int n_steps = 3, double max = 1);
    void Configure(double n_steps, double max);

public:
    void SetLevel(double value);
     void SetEnabled(bool flag);

//signals:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void SetupLevels();
    QColor GetColor(int step) const;
    void DrawPole(QPainter *painter);
    std::vector<double> levels_;
    std::vector<QColor> colors_;
    std::vector<QColor> base_colors_ = {Qt::green, Qt::yellow, Qt::red};
    QColor COLOR_DISABLED = QRgb(0x454545);

    int n_steps_;
    double max_;
    double value_ = 0.0;
    bool enabled_ = true;
};

#endif
