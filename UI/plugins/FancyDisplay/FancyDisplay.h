#ifndef FancyDisplay_H
#define FancyDisplay_H

#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QColor>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT FancyDisplay : public QWidget
{
    Q_OBJECT
public:
    enum class GeometricShape { ELLIPS };
    explicit FancyDisplay(QWidget *parent = 0,
                          GeometricShape shape = GeometricShape::ELLIPS,
                          QColor color = Qt::blue,
                          int width = 200, int height = 100, double angle = 0,
                          QString unit = "-");

public:

signals:
    void SIGNAL_Clicked();
    void SIGNAL_ValueChanged();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent ( QMouseEvent * event ) Q_DECL_OVERRIDE;

public slots:
    void SetValue(double value);

private slots:
    void SetDefaultColor();
    void SetClickedColor();

private:
    void DrawEllips(QPainter *painter);
    GeometricShape shape_;
    QBrush standard_brush_;
    QBrush active_brush_;
    QBrush clicked_brush_ = QBrush(Qt::black);
    double width_;
    double height_;
    double angle_;
    double display_value_ = 0.0;
    QString unit_;
    QTimer color_change_timer_;
};

#endif
