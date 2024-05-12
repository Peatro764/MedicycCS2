#ifndef BITWIDGET_H
#define BITWIDGET_H

#include <QBrush>
#include <QPen>
#include <QWidget>

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

class BitWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BitWidget(bool read_only, QWidget *parent = 0);

signals:
    void SIGNAL_Changed(bool on);

public slots:
    bool On() const;
    void SetEnabled(bool yes);
    void SetOn(bool yes);

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QBrush brush_;
    QPen pen_;
    bool enabled_ = false;
    bool on_ = false;
    bool read_only_ = false;
};

} // ns

#endif
