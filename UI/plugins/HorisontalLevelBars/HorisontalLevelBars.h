#ifndef HorisontalLevelBars_H
#define HorisontalLevelBars_H

#include <QTime>
#include <QWidget>
#include <QDebug>
#include <QColor>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT HorisontalLevelBars : public QWidget
{
    Q_OBJECT
public:
    explicit HorisontalLevelBars(QWidget *parent = 0, int width = 100, int height = 100, QBrush on = QBrush(Qt::blue), QBrush off = QBrush(Qt::black), std::vector<int> steps = {1, 2, 3});

public slots:
    void ResetLevel();
    int ActiveLevel() const { return steps_.at(act_step_); }

signals:
    void SIGNAL_Clicked();
    void SIGNAL_ActiveLevelChanged(int);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent ( QMouseEvent * event ) Q_DECL_OVERRIDE;

private slots:
    void DoOneStep();

private:
    void DrawBars(QPainter *painter);
    QBrush brush_on_;
    QBrush brush_off_;
    std::vector<int> steps_;
    int act_step_ = 0;
};

#endif
