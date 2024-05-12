#include "EquipmentListGui.h"

#include <QGridLayout>
#include <QDebug>

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

EquipmentListGui::EquipmentListGui(QStringList equipment)
{
    qDebug() << "EquipmentListGui";
    setWindowTitle("EQUIPEMENTS");
    setFixedSize(1280, 770);
    SetupUI(equipment);
}

EquipmentListGui::~EquipmentListGui()
{
}

void EquipmentListGui::SetupUI(QStringList equipment) {
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    const int NCOL = 6;
    for (int idx = 0; idx < equipment.size(); ++idx) {
        int col = idx % NCOL;
        int row = idx / NCOL;
        QString name = equipment.at(idx);
        QPushButton *pushButton = GetSelectionPushButton(name);
        QObject::connect(pushButton, &QPushButton::clicked, this, [&, name]() { emit SIGNAL_EquipmentSelected(name); });
        gridLayout->addWidget(pushButton, row, col);
    }
    QPushButton *pushButton = GetSelectionPushButton("Quitter?");
    int colSpan = (NCOL - equipment.size() % NCOL);
    gridLayout->addWidget(pushButton, equipment.size() / NCOL, equipment.size() % NCOL, 1, colSpan);
    QObject::connect(pushButton, &QPushButton::clicked, this, &EquipmentListGui::close);
    this->setLayout(gridLayout);
}

QPushButton* EquipmentListGui::GetSelectionPushButton(QString name) const {
    QPushButton *pushButton = new QPushButton(QString(name).replace("_", " "));
    pushButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return pushButton;
}

}
