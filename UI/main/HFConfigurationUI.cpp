#include "HFConfigurationUI.h"

#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>

#include "Utils.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

HFConfigurationUI::HFConfigurationUI(QWidget *parent) :
    QWidget(parent),
    repo_("_prod")
{
    qDebug() << "HFConfigurationGui::HFConfigurationGui";

    // Central rows
    QGridLayout* grid_layout = SetupParameterGrid();

    // Bottom row
    auto pushButton_save =  shared::utils::GetPushButton("Sauvegarder", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_save, &QPushButton::clicked, this, &HFConfigurationUI::SaveParameters);
    auto pushButton_close =  shared::utils::GetPushButton("Quitter", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_close, &QPushButton::clicked, this, &HFConfigurationUI::close);
    QHBoxLayout *h_layout = new QHBoxLayout;
    h_layout->addWidget(pushButton_close);
    h_layout->addWidget(pushButton_save);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(grid_layout);
    v_layout->addLayout(h_layout);
}

HFConfigurationUI::~HFConfigurationUI() {
    qDebug() << "HFConfigurationUI::~HFConfigurationUI";
}

void HFConfigurationUI::SaveParameters() {
    try {
        repo_.SaveConfiguration(conf_hf1_);
        repo_.SaveConfiguration(conf_hf2_);
        QMessageBox::information(this, "MainGui", "Mise à jour du système avec les nouveaux paramètres réussie");

    } catch (std::exception& exc) {
        qWarning() << "HFConfigurationUI::SaveParameters exception thrown " << exc.what();
        QMessageBox::critical(this, "MainGui", "Échec de la mise à jour du système avec les nouveaux paramètres");
    }
}

QGridLayout* HFConfigurationUI::SetupParameterGrid() {
    QGridLayout *gridLayout = new QGridLayout;

    // Top labels
    gridLayout->addWidget(shared::utils::GetLabel("Paramètre", shared::FONT::MEDIUM_LABEL), 0, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("HF1", shared::FONT::MEDIUM_LABEL), 0, 1, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("HF2", shared::FONT::MEDIUM_LABEL), 0, 2, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Unité", shared::FONT::MEDIUM_LABEL), 0, 3, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Commentaires", shared::FONT::MEDIUM_LABEL), 0, 4, Qt::AlignHCenter);

    // Variable names
    gridLayout->addWidget(shared::utils::GetLabel("Accord tension bas", shared::FONT::SMALL_LABEL), 1, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Accord tension haut", shared::FONT::SMALL_LABEL), 2, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Accord phase bas", shared::FONT::SMALL_LABEL), 3, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Accord phase haut", shared::FONT::SMALL_LABEL), 4, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Membrane minimale", shared::FONT::SMALL_LABEL), 5, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Membrane maximale", shared::FONT::SMALL_LABEL), 6, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Membrane dégagement", shared::FONT::SMALL_LABEL), 7, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau on", shared::FONT::SMALL_LABEL), 8, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau off", shared::FONT::SMALL_LABEL), 9, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau final démarrage", shared::FONT::SMALL_LABEL), 10, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau initial shutdown", shared::FONT::SMALL_LABEL), 11, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau maximale", shared::FONT::SMALL_LABEL), 12, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension maximale", shared::FONT::SMALL_LABEL), 13, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension delta dec", shared::FONT::SMALL_LABEL), 14, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension delta inc", shared::FONT::SMALL_LABEL), 15, 0, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension post accord", shared::FONT::SMALL_LABEL), 16, 0, Qt::AlignLeft);

    // Units
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 1, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 2, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("-", shared::FONT::SMALL_LABEL), 3, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("-", shared::FONT::SMALL_LABEL), 4, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("%", shared::FONT::SMALL_LABEL), 5, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("%", shared::FONT::SMALL_LABEL), 6, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("%", shared::FONT::SMALL_LABEL), 7, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("div", shared::FONT::SMALL_LABEL), 8, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("div", shared::FONT::SMALL_LABEL), 9, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("div", shared::FONT::SMALL_LABEL), 10, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("div", shared::FONT::SMALL_LABEL), 11, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("div", shared::FONT::SMALL_LABEL), 12, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 13, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 14, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 15, 3, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("kV", shared::FONT::SMALL_LABEL), 16, 3, Qt::AlignLeft);

    // Comments
    gridLayout->addWidget(shared::utils::GetLabel("Tension dee minimale pour membrane accord", shared::FONT::SMALL_LABEL), 1, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension dee maximale pendant la recherche d'accord", shared::FONT::SMALL_LABEL), 2, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Phase minimale pour accord membrane", shared::FONT::SMALL_LABEL), 3, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Phase maximale pour accord membrane", shared::FONT::SMALL_LABEL), 4, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Limite inférieure de membrane", shared::FONT::SMALL_LABEL), 5, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Limite supérieure de membrane", shared::FONT::SMALL_LABEL), 6, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Distance de la limite de membrane où le démarrage/l'arrêt peut reprendre", shared::FONT::SMALL_LABEL), 7, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Valeur du niveau avant de l'activer", shared::FONT::SMALL_LABEL), 8, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Valeur du niveau avant de le desactiver", shared::FONT::SMALL_LABEL), 9, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau final au démarrage", shared::FONT::SMALL_LABEL), 10, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau initial pendant l'arrêt", shared::FONT::SMALL_LABEL), 11, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Niveau maximal", shared::FONT::SMALL_LABEL), 12, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension dee maximale pendant la procédure de démarrage", shared::FONT::SMALL_LABEL), 13, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Décréments tension dee pendant la procédure d'arrêt", shared::FONT::SMALL_LABEL), 14, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Incréments tension dee pendant la procédure de démarrage", shared::FONT::SMALL_LABEL), 15, 4, Qt::AlignLeft);
    gridLayout->addWidget(shared::utils::GetLabel("Tension dee réglée une fois l'accord trouvé", shared::FONT::SMALL_LABEL), 16, 4, Qt::AlignLeft);

    try {
        conf_hf1_ = repo_.GetLastConfiguration(hf_middlelayer::HFX::HF1);
        conf_hf2_ = repo_.GetLastConfiguration(hf_middlelayer::HFX::HF2);
        qDebug() << "HFConfigurationGui::SetupParameterGrid Got configs " << conf_hf1_.timestamp() << " " << conf_hf2_.timestamp();

        // hf1 values
        auto spin_hf1_accord_voltage_min = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.accord_voltage_lower(), 0.0, 20.0);
        gridLayout->addWidget(spin_hf1_accord_voltage_min, 1, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_accord_voltage_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_accord_voltage_lower(value); });
        auto spin_hf1_accord_voltage_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.accord_voltage_upper(), 0.0, 45.0);
        gridLayout->addWidget(spin_hf1_accord_voltage_max, 2, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_accord_voltage_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_accord_voltage_upper(value); });
        auto spin_hf1_accord_phase_min = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.accord_phase_lower(), 0.0, 15.0);
        gridLayout->addWidget(spin_hf1_accord_phase_min, 3, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_accord_phase_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_accord_phase_lower(value); });
        auto spin_hf1_accord_phase_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.accord_phase_upper(), 0.0, 15.0);
        gridLayout->addWidget(spin_hf1_accord_phase_max, 4, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_accord_phase_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_accord_phase_upper(value); });
        auto spin_hf1_membrane_min = shared::utils::GetDoubleSpinBox(1, 0, conf_hf1_.membrane_min(), 0, 90);
        gridLayout->addWidget(spin_hf1_membrane_min, 5, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_membrane_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_membrane_min(value); });
        auto spin_hf1_membrane_max = shared::utils::GetDoubleSpinBox(1, 0, conf_hf1_.membrane_max(), 0, 90);
        gridLayout->addWidget(spin_hf1_membrane_max, 6, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_membrane_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_membrane_max(value); });
        auto spin_hf1_membrane_clearance = shared::utils::GetDoubleSpinBox(1, 0, conf_hf1_.membrane_clearance(), 0, 30);
        gridLayout->addWidget(spin_hf1_membrane_clearance, 7, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_membrane_clearance, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_membrane_clearance(value); });
        auto spin_hf1_niveau_preon = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.niveau_preon_value(), 0, 5);
        gridLayout->addWidget(spin_hf1_niveau_preon, 8, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_niveau_preon, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_niveau_preon_value(value); });
        auto spin_hf1_niveau_preoff = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.niveau_preoff_value(), 0, 5);
        gridLayout->addWidget(spin_hf1_niveau_preoff, 9, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_niveau_preoff, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_niveau_preoff_value(value); });
        auto spin_hf1_niveau_startup_final = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.niveau_startup_final_value(), 0, 8);
        gridLayout->addWidget(spin_hf1_niveau_startup_final, 10, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_niveau_startup_final, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_niveau_startup_final_value(value); });
        auto spin_hf1_niveau_shutdown_initial = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.niveau_shutdown_initial_value(), 0, 8);
        gridLayout->addWidget(spin_hf1_niveau_shutdown_initial, 11, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_niveau_shutdown_initial, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_niveau_shutdown_initial_value(value); });
        auto spin_hf1_niveau_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.niveau_max() , 0, 10);
        gridLayout->addWidget(spin_hf1_niveau_max, 12, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_niveau_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_niveau_max(value); });
        auto spin_hf1_voltage_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.voltage_max(), 0, 60);
        gridLayout->addWidget(spin_hf1_voltage_max, 13, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_voltage_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_voltage_max(value); });
        auto spin_hf1_voltage_delta_down = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.voltage_delta_down(), 0, 20);
        gridLayout->addWidget(spin_hf1_voltage_delta_down, 14, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_voltage_delta_down, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_voltage_delta_down(value); });
        auto spin_hf1_voltage_delta_up = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.voltage_delta_up(), 0, 20);
        gridLayout->addWidget(spin_hf1_voltage_delta_up, 15, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_voltage_delta_up, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_voltage_delta_up(value); });
        auto spin_hf1_voltage_post_accord = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf1_.voltage_post_accord(), 5, 25);
        gridLayout->addWidget(spin_hf1_voltage_post_accord, 16, 1, Qt::AlignCenter);
        QObject::connect(spin_hf1_voltage_post_accord, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf1_.set_voltage_post_accord(value); });

        // hf2 values
        auto spin_hf2_accord_voltage_min = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.accord_voltage_lower(), 0.0, 20.0);
        gridLayout->addWidget(spin_hf2_accord_voltage_min, 1, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_accord_voltage_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_accord_voltage_lower(value); });
        auto spin_hf2_accord_voltage_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.accord_voltage_upper(), 0.0, 45.0);
        gridLayout->addWidget(spin_hf2_accord_voltage_max, 2, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_accord_voltage_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_accord_voltage_upper(value); });
        auto spin_hf2_accord_phase_min = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.accord_phase_lower(), 0.0, 15.0);
        gridLayout->addWidget(spin_hf2_accord_phase_min, 3, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_accord_phase_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_accord_phase_lower(value); });
        auto spin_hf2_accord_phase_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.accord_phase_upper(), 0.0, 15.0);
        gridLayout->addWidget(spin_hf2_accord_phase_max, 4, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_accord_phase_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_accord_phase_upper(value); });
        auto spin_hf2_membrane_min = shared::utils::GetDoubleSpinBox(1, 0, conf_hf2_.membrane_min(), 0, 90);
        gridLayout->addWidget(spin_hf2_membrane_min, 5, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_membrane_min, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_membrane_min(value); });
        auto spin_hf2_membrane_max = shared::utils::GetDoubleSpinBox(1, 0, conf_hf2_.membrane_max(), 0, 90);
        gridLayout->addWidget(spin_hf2_membrane_max, 6, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_membrane_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_membrane_max(value); });
        auto spin_hf2_membrane_clearance = shared::utils::GetDoubleSpinBox(1, 0, conf_hf2_.membrane_clearance(), 0, 30);
        gridLayout->addWidget(spin_hf2_membrane_clearance, 7, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_membrane_clearance, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_membrane_clearance(value); });
        auto spin_hf2_niveau_preon = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.niveau_preon_value(), 0, 5);
        gridLayout->addWidget(spin_hf2_niveau_preon, 8, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_niveau_preon, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_niveau_preon_value(value); });
        auto spin_hf2_niveau_preoff = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.niveau_preoff_value(), 0, 5);
        gridLayout->addWidget(spin_hf2_niveau_preoff, 9, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_niveau_preoff, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_niveau_preoff_value(value); });
        auto spin_hf2_niveau_startup_final = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.niveau_startup_final_value(), 0, 8);
        gridLayout->addWidget(spin_hf2_niveau_startup_final, 10, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_niveau_startup_final, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_niveau_startup_final_value(value); });
        auto spin_hf2_niveau_shutdown_initial = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.niveau_shutdown_initial_value(), 0, 8);
        gridLayout->addWidget(spin_hf2_niveau_shutdown_initial, 11, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_niveau_shutdown_initial, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_niveau_shutdown_initial_value(value); });
        auto spin_hf2_niveau_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.niveau_max() , 0, 10);
        gridLayout->addWidget(spin_hf2_niveau_max, 12, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_niveau_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_niveau_max(value); });
        auto spin_hf2_voltage_max = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.voltage_max(), 0, 60);
        gridLayout->addWidget(spin_hf2_voltage_max, 13, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_voltage_max, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_voltage_max(value); });
        auto spin_hf2_voltage_delta_down = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.voltage_delta_down(), 0, 20);
        gridLayout->addWidget(spin_hf2_voltage_delta_down, 14, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_voltage_delta_down, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_voltage_delta_down(value); });
        auto spin_hf2_voltage_delta_up = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.voltage_delta_up(), 0, 20);
        gridLayout->addWidget(spin_hf2_voltage_delta_up, 15, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_voltage_delta_up, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_voltage_delta_up(value); });
        auto spin_hf2_voltage_post_accord = shared::utils::GetDoubleSpinBox(0.1, 1, conf_hf2_.voltage_post_accord(), 5, 25);
        gridLayout->addWidget(spin_hf2_voltage_post_accord, 16, 2, Qt::AlignCenter);
        QObject::connect(spin_hf2_voltage_post_accord, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) { conf_hf2_.set_voltage_post_accord(value); });

    } catch (std::exception& exc) {
        qWarning() << "HFConfigurationUI::SetupParameterGrid " << exc.what();
        QMessageBox::critical(this, "MainGui", "Échec de l'obtention des paramètres HF");
    }

    return gridLayout;
}



}
