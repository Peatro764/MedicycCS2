#ifndef GENERICDISPLAYBUTTON_H
#define GENERICDISPLAYBUTTON_H

#include <QTime>
#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>

class QDESIGNER_WIDGET_EXPORT GenericDisplayButton : public QWidget
{
//    Q_OBJECT

public:
    GenericDisplayButton(QWidget *parent = 0, int side = 100);

public:
    void SetupAsColorChanging(QColor on, QColor off);
    void SetupAsCenterFilling(QColor color);
    void On();
    void Off();
    void Enable();
    void Disable();
    QString text() const;

//signals:

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    void SetEnable(bool flag);
    void DrawCenter(QPainter *painter, QColor color);
    void DrawArc(QPainter *painter, QColor color);

    QColor COLOR_GREEN = Qt::green;
    QColor COLOR_BLUE = QRgb(0x209fdf);
    QColor COLOR_RED = Qt::red;
    QColor COLOR_DISABLED = QRgb(0x454545);

    QColor color_on_ = Qt::yellow;
    QColor color_off_ = Qt::yellow;

    bool enabled_ = false;
    bool on_ = false;
    bool color_changing_button_ = false;
};

#endif
