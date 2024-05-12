#include "PrintWidget.h"

#include <QGridLayout>
#include <QDebug>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>

#include "Utils.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

PrintWidget::PrintWidget(std::vector<EquipmentGroupValues> group_values)
    : group_values_(group_values)
{
    qDebug() << "PrintWidget::PrintWidget";
    setWindowTitle("Etat du cyclotron");
    // Override default colors since it should be printed (and thus avoid colors...)
    this->setStyleSheet("background: white; color: black;");
    setFixedSize(1.75*595, 1.75*842);
    SetupUI();
}

PrintWidget::~PrintWidget()
{
}

void PrintWidget::SetupUI() {
    QGridLayout *gridLayout = new QGridLayout(this);
    QLabel *lTopTitle = shared::utils::GetLabel("MEDICYC", QFont("Arial", 30), LABEL_COLOR);
    QLabel *lDate = shared::utils::GetLabel(QDateTime::currentDateTime().toString() , QFont("Arial", 16), LABEL_COLOR);

    gridLayout->addWidget(lTopTitle);
    gridLayout->addWidget(lDate);

    for (EquipmentGroupValues& group_value : group_values_) {
        QWidget *w = CreateGroupValueWidget(group_value);
        gridLayout->addWidget(w);
    }
    this->setLayout(gridLayout);
}

QWidget* PrintWidget::CreateGroupValueWidget(EquipmentGroupValues group_value) const {
    QFrame *frame = new QFrame;
    QLabel *lTitle = shared::utils::GetLabel(group_value.GetSubSystem(), QFont("Arial", 15), LABEL_COLOR);
    QLabel *lEquipment = shared::utils::GetLabel("Equipment", QFont("Arial", 13), LABEL_COLOR);
    QLabel *lEtat = shared::utils::GetLabel("Etat", QFont("Arial", 13), LABEL_COLOR);
    QLabel *lValeurDes = shared::utils::GetLabel("Consigné", QFont("Arial", 13), LABEL_COLOR);
    QLabel *lValeurAct = shared::utils::GetLabel("Valeur", QFont("Arial", 13), LABEL_COLOR);
    QLabel *lValeurDiff = shared::utils::GetLabel("Delta", QFont("Arial", 13), LABEL_COLOR);

    QGridLayout *gLayout = new QGridLayout;
    gLayout->setColumnMinimumWidth(0, 350);
    frame->setLayout(gLayout);
    QMap<QString, EquipmentValues> values = group_value.GetValues();
    int irow = 0;
    int icol = 0;
    gLayout->addWidget(lTitle, irow++, icol, Qt::AlignLeft);
    gLayout->addWidget(lEquipment, irow, icol++, Qt::AlignLeft);
    gLayout->addWidget(lEtat, irow, icol++, Qt::AlignHCenter);
    gLayout->addWidget(lValeurDes, irow, icol++, Qt::AlignHCenter);
    gLayout->addWidget(lValeurAct, irow, icol++, Qt::AlignHCenter);
    gLayout->addWidget(lValeurDiff, irow, icol++, Qt::AlignHCenter);
    irow++;

    for (auto k : values.keys()) {
        icol = 0;
        EquipmentValues eq_value = values.value(k);
        gLayout->addWidget(shared::utils::GetLabel(k, QFont("Arial", 12), LABEL_COLOR), irow, icol++, Qt::AlignLeft);
        gLayout->addWidget(shared::utils::GetLabel(eq_value.state(), QFont("Arial", 12), LABEL_COLOR), irow, icol++, Qt::AlignHCenter);
        gLayout->addWidget(shared::utils::GetLabel(eq_value.des_value(), QFont("Arial", 12), LABEL_COLOR), irow, icol++, Qt::AlignHCenter);
        gLayout->addWidget(shared::utils::GetLabel(eq_value.act_value(), QFont("Arial", 12), LABEL_COLOR), irow, icol++, Qt::AlignHCenter);
        bool act_value_ok(false);
        bool des_value_ok(false);
        double act_value = eq_value.act_value().toDouble(&act_value_ok);
        double des_value = eq_value.des_value().toDouble(&des_value_ok);
        QString diff_value("-");
        if ((eq_value.state() == QString("ON")) && act_value_ok && des_value_ok) {
            diff_value = QString::number(act_value - des_value, 'f', 2);
        }
        gLayout->addWidget(shared::utils::GetLabel(diff_value, QFont("Arial", 12), LABEL_COLOR), irow, icol++, Qt::AlignHCenter);
        irow++;
    }
    return frame;
}

}
