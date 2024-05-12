#ifndef BITFIELDWIDGET_H
#define BITFIELDWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMap>

#include "BitWidget.h"

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::standard {

enum class BITFIELDALIGNMENT { LEFT, RIGHT, SPARSE };

class BitFieldWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BitFieldWidget(QWidget *parent = 0);
    void Setup(int nbitsmax, int nbits, bool read_only, BITFIELDALIGNMENT alignment);

signals:
    void SIGNAL_Changed(int value);

public slots:
    int GetValue() const;
    void SetValue(int value);
    void Enable(int bit);
    void Disable(int bit);

private slots:
    void TearDown();
    void Changed();

protected:
    void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
    QMap<int, BitWidget*> bits_;
};

} // ns

#endif
