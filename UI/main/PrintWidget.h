#ifndef PRINTWIDGET_H
#define PRINTWIDGET_H

#include <QWidget>
#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QWidget>

#include "EquipmentGroupValues.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

class PrintWidget : public QWidget
{
    Q_OBJECT

public:
    PrintWidget(std::vector<EquipmentGroupValues> group_values);
    ~PrintWidget();

public slots:

private slots:

signals:

private:
    void SetupUI();
    QWidget* CreateGroupValueWidget(EquipmentGroupValues group_values) const;
    std::vector<EquipmentGroupValues> group_values_;
    const QString LABEL_COLOR = "black";
};

}

#endif
