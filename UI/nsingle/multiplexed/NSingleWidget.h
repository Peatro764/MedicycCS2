#ifndef NSINGLEWIDGET_H
#define NSINGLEWIDGET_H

#include <QWidget>
#include <memory>
#include <QSettings>
#include <QVBoxLayout>
#include <QTimer>
#include <QFrame>
#include <QLabel>

#include "ClickableFrame.h"

namespace medicyc::cyclotroncontrolsystem::ui::nsingle::multiplexed {

class NSingleWidget : public QObject
{
    Q_OBJECT

public:
    explicit NSingleWidget(QString name);

    QVBoxLayout* GetLayout() { return layout_; }

public slots:
    QString name() { return name_; }
    void Ping();
    void Connected();
    void Disconnected();
    void Toggle();
    void Select();
    void Deselect();

signals:
    void Selected(QString name);
    void Deselected(QString name);

private slots:
    void Draw();

private:
    void SetupConnections();
    QVBoxLayout* CreateLayout();

    QVBoxLayout* layout_;
    QLabel* label_;
    ClickableFrame* frame_connection_;
    QFrame* frame_ping_;
    QString name_;
    QTimer timer_;
    bool selected_ = false;
    bool connected_ = false;
};

} // ns

#endif
