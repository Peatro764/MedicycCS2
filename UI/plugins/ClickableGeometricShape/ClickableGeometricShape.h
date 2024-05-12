#ifndef ClickableGeometricShape_H
#define ClickableGeometricShape_H

#include <QTime>
#include <QTimer>
#include <QWidget>
#include <QColor>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT ClickableGeometricShape : public QWidget
{
    Q_OBJECT
public:
    enum class GeometricShape { DOUBLEARROW };
    explicit ClickableGeometricShape(QWidget *parent = 0, GeometricShape shape = GeometricShape::DOUBLEARROW, QBrush brush = QBrush(Qt::blue), int width = 100, int height = 100, double angle = 0);

public:

signals:
    void SIGNAL_Clicked();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent ( QMouseEvent * event ) Q_DECL_OVERRIDE;

private slots:
    void SetDefaultColor();
    void SetClickedColor();

private:
    void DrawDoubleArrow(QPainter *painter);
    GeometricShape shape_;
    QBrush standard_brush_;
    QBrush active_brush_;
    QBrush clicked_brush_ = QBrush(Qt::black);
    double width_;
    double height_;
    double angle_;
    QTimer color_change_timer_;
};

#endif
