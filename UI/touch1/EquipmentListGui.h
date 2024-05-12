#ifndef EQUIPMENTLISTGUI_H
#define EQUIPMENTLISTGUI_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPushButton>

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

class EquipmentListGui : public QWidget
{
    Q_OBJECT

public:
    explicit EquipmentListGui(QStringList equipment);
    ~EquipmentListGui();

public slots:

private slots:

signals:
    void SIGNAL_EquipmentSelected(QString name);

private:
    void SetupUI(QStringList equipment);
    QPushButton* GetSelectionPushButton(QString name) const;
};

}

#endif
