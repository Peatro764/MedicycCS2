#include "HFUI.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QSound>
#include <QInputDialog>
#include <QMessageBox>

#include "FancyDisplay.h"
#include "GenericDisplayButton.h"
#include "ClickableGeometricShape.h"
#include "Utils.h"
#include "DBus.h"
#include "Style.h"
#include "HFUtils.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

HFUI::HFUI(QWidget *parent) :
    QWidget(parent),
    powersupplies_interface_(medicyc::cyclotron::HFPowerSuppliesInterface(QString("medicyc.cyclotron.hardware.hfpowersupplies"), QString("/PowerSupplies"), medicyc::cyclotroncontrolsystem::global::GetDBusConnection())),
    controller_interface_("medicyc.cyclotron.middlelayer.hf", "/Controller", medicyc::cyclotroncontrolsystem::global::GetDBusConnection())
{
    SetupUI();
    qDebug() << "HF PowerSupplies DBUS " << powersupplies_interface_.isValid();
    qDebug() << "HF Controller DBUS " << controller_interface_.isValid();

    // Relay group state to main gui
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_StartupFinished, this, &HFUI::SIGNAL_StartupFinished);
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_ShutdownFinished, this, &HFUI::SIGNAL_ShutdownFinished);
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_State_Off, this, &HFUI::SIGNAL_State_Off);
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_State_Intermediate, this, &HFUI::SIGNAL_State_Intermediate);
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_State_Ready, this, &HFUI::SIGNAL_State_Ready);
    QObject::connect(&controller_interface_, &middlelayer::HFControllerInterface::SIGNAL_State_Unknown, this, &HFUI::SIGNAL_State_Unknown);
}

HFUI::~HFUI() {
}

void HFUI::Startup() {
}

void HFUI::Shutdown() {
}

void HFUI::Interrupt() {
    controller_interface_.Interrupt();
}

void HFUI::Configure(QString name) {
    (void)name;
    qDebug() << "HFUI::Configure Not implemented for HF";
}

void HFUI::StartupAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Démarrer la HF?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { controller_interface_.Startup(); }
}

void HFUI::ShutdownAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Arrêter la HF?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { controller_interface_.Shutdown(); }
}

void HFUI::SetupUI() {
    // Top row (title and configuration button)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("HF", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QPushButton *p_config = new QPushButton;
    p_config->setObjectName("p_config");
    p_config->setStyleSheet("QPushButton#p_config {"
                            "background: transparent;"
                            "background-image: url(:/images/configuration.png);"
                            "background-repeat: no-repeat;"
                            "background-position: center;"
                            "border: none;"
                            "};");
    QObject::connect(p_config, &QPushButton::clicked, this, &HFUI::OpenConfigurationGui);

    p_config->setMaximumHeight(45);
    p_config->setMaximumWidth(45);
    p_config->setMinimumHeight(45);
    p_config->setMinimumWidth(45);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(p_config, Qt::AlignRight | Qt::AlignVCenter);


    // Line separator
    auto line_1 = new QFrame;
    line_1->setFixedHeight(3);
    line_1->setFrameShadow(QFrame::Sunken);
    line_1->setLineWidth(2);
    line_1->setStyleSheet("background-color: cornflowerblue");

    // Central rows
    QGridLayout* grid_layout = new QGridLayout;
    int row = 0;
    int spacer = 1;
    grid_layout->addItem(new QSpacerItem(spacer, spacer, QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
    grid_layout->addLayout(CreateGeneratorWindow(), row++, 0, 1, 2);
    grid_layout->addItem(new QSpacerItem(spacer, spacer, QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
    grid_layout->addLayout(CreateTitleWindow(), row++, 0, 1, 2);
    grid_layout->addItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
    grid_layout->addLayout(CreatePowerSuppliesStatusLayout(), row++, 0, 1, 2);
    grid_layout->addItem(new QSpacerItem(10, 10, QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
    grid_layout->addWidget(CreateNiveauWindow(medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX::HF1), row, 0);
    grid_layout->addWidget(CreateNiveauWindow(medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX::HF2), row++, 1);
    grid_layout->addItem(new QSpacerItem(spacer, spacer, QSizePolicy::Fixed, QSizePolicy::Fixed), row++, 0, 1, 2);
    grid_layout->addLayout(CreatePowerSuppliesCommandLayout(), row++, 0, 1, 2);

    // Bottom rows (control pushbuttons)
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    auto pushButton_startup =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_shutdown =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_interrupt =  shared::utils::GetPushButton("Interrompre", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_startup, &QPushButton::clicked, this, &HFUI::StartupAfterConfirmation);
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, &HFUI::ShutdownAfterConfirmation);
    QObject::connect(pushButton_interrupt, &QPushButton::clicked, this, &HFUI::Interrupt);
    bottom_layout->addWidget(pushButton_startup);
    bottom_layout->addWidget(pushButton_shutdown);
    bottom_layout->addWidget(pushButton_interrupt);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout);
    v_layout->addSpacing(20);
    v_layout->addLayout(bottom_layout);
}

QHBoxLayout* HFUI::CreateTitleWindow() {
    QHBoxLayout *g_layout = new QHBoxLayout;
    QLabel *l_hf1 = shared::utils::GetLabel("HF1", shared::FONT::XXXLARGE_LABEL);
    QLabel *l_hf2 = shared::utils::GetLabel("HF2", shared::FONT::XXXLARGE_LABEL);
    l_hf1->setAlignment(Qt::AlignCenter);
    l_hf2->setAlignment(Qt::AlignCenter);
    l_hf1->setMaximumHeight(70);
    l_hf2->setMaximumHeight(70);
    g_layout->addWidget(l_hf1);
    g_layout->addWidget(l_hf2);
    return g_layout;
}

QVBoxLayout* HFUI::CreateGeneratorWindow() {

    QVBoxLayout *g_layout = new QVBoxLayout;

    QFont title_font = QFont("URW Chancery L", 18);
    QLabel *l_title = shared::utils::GetLabel("f", shared::FONT::SMALL_LABEL);
    l_title->setAlignment(Qt::AlignCenter);
    l_title->setFont(title_font);
    l_title->setMaximumHeight(30);

    // Init, Set F buttons
    auto pushButton_gene_init =  shared::utils::GetPushButton("Init", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_gene_init, &QPushButton::clicked, this, [&]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "HF", "Cela initialisera le générateur et réglera la fréquence à 25,01 MHz. Voulez-vous poursuivre?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (reply == QMessageBox::Yes) controller_interface_.InitGenerator();
    });
    auto pushButton_gene_set =  shared::utils::GetPushButton("Set", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_gene_set, &QPushButton::clicked, this, [&]() {
        bool ok(false);
        double frequency = QInputDialog::getDouble(this, "Génerateur", "Nouvelle fréquence",
                                                   25.01, 24.80, 25.1, 2, &ok, Qt::WindowFlags(), 0.01);
        if (ok) { controller_interface_.SetFrequency(frequency); }
    });

    // Inc/dec buttons
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, 0x209fdf);
    gradient.setColorAt(1.0, 0x0E6692);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    ClickableGeometricShape *incButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 40, 40, 180);
    ClickableGeometricShape *decButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 40, 40, 0);
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { controller_interface_.DecFrequency(); });
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { controller_interface_.IncFrequency(); });

    FancyDisplay *frequency_display = new FancyDisplay(this, FancyDisplay::GeometricShape::ELLIPS, QColor("cornflowerblue"),
                                                       300, 80, 0, "MHz");
    QObject::connect(&controller_interface_, &medicyc::cyclotron::middlelayer::HFControllerInterface::SIGNAL_Frequency,
                     this, [&, frequency_display](double mhz) { frequency_display->SetValue(mhz); });
    QObject::connect(frequency_display, &FancyDisplay::SIGNAL_ValueChanged, this, &HFUI::SIGNAL_FrequencyChanged);


    QHBoxLayout* layout = new QHBoxLayout;
    layout->addWidget(pushButton_gene_init, Qt::AlignRight);
    layout->addSpacing(20);
    layout->addWidget(decButton);
    layout->addWidget(frequency_display);
    layout->addWidget(incButton);
    layout->addSpacing(20);
    layout->addWidget(pushButton_gene_set, Qt::AlignRight);

    g_layout->addWidget(l_title);
    g_layout->addLayout(layout);

    return g_layout;
}

QFrame* HFUI::CreateNiveauWindow(medicyc::cyclotroncontrolsystem::middlelayer::hf::HFX hf) {
    QString HF = QString("HF");
    HF.append(QString::number(static_cast<int>(hf)));

    // Voltage Dee layout
    QHBoxLayout* dee_layout = new QHBoxLayout;
    QLabel *l_voltage_text = shared::utils::GetLabel("V Dee " + QString::number(static_cast<int>(hf)), shared::FONT::LARGE_LABEL);
    QLabel *l_voltage_value = shared::utils::GetLabel("-", shared::FONT::XXLARGE_LABEL);
    l_voltage_text->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    l_voltage_value->setAlignment(Qt::AlignCenter);
    dee_layout->addWidget(l_voltage_text);
    dee_layout->addWidget(l_voltage_value);
    dee_layout->addSpacing(30);
    QObject::connect(&controller_interface_, &medicyc::cyclotron::middlelayer::HFControllerInterface::SIGNAL_Dee_Voltage,
                     this, [&, hf, l_voltage_value](int branch, double value) {
        if (static_cast<int>(hf) == branch) l_voltage_value->setText(QString::number(value, 'f', 1) + " kV");
    });

    // Niveau layout
    QHBoxLayout* niveau_layout = new QHBoxLayout;

    // Inc/dec buttons
    QVBoxLayout* v_layout_left = new QVBoxLayout;
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, 0x209fdf);
    gradient.setColorAt(1.0, 0x0E6692);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    ClickableGeometricShape *incButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 40, 40, 180);
    ClickableGeometricShape *decButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 40, 40, 0);
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this, hf]() { controller_interface_.IncNiveau(static_cast<int>(hf)); });
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this, hf]() { controller_interface_.DecNiveau(static_cast<int>(hf)); });
    v_layout_left->addWidget(incButton);
    v_layout_left->addWidget(decButton);

    // Niveau text and value
    QVBoxLayout* v_layout_right = new QVBoxLayout;
    QLabel *l_niveau_text = shared::utils::GetLabel("Niveau", shared::FONT::LARGE_LABEL);
    QLabel *l_niveau_value = shared::utils::GetLabel("-", shared::FONT::XXLARGE_LABEL);
    l_niveau_text->setAlignment(Qt::AlignCenter);
    l_niveau_value->setAlignment(Qt::AlignCenter);
    v_layout_right->addWidget(l_niveau_text);
    v_layout_right->addWidget(l_niveau_value);
    v_layout_right->addSpacing(30);
    QObject::connect(&controller_interface_, &medicyc::cyclotron::middlelayer::HFControllerInterface::SIGNAL_Niveau_Value,
                     this, [&, hf, l_niveau_value](int branch, double value) {
        if (static_cast<int>(hf) == branch) l_niveau_value->setText(QString::number(value, 'f', 1) + " div");
    });
    niveau_layout->addLayout(v_layout_left);
    niveau_layout->addLayout(v_layout_right);

    // Global layout
    QVBoxLayout *g_layout = new QVBoxLayout;
    g_layout->addLayout(dee_layout);
    g_layout->addLayout(niveau_layout);

    // Wrap in frame for borders
    QFrame *g_frame = new QFrame;
    g_frame->setMaximumHeight(200);
    g_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    g_frame->setObjectName("niveau_values_frame_" + HF);
    g_frame->setStyleSheet("QFrame#" + g_frame->objectName() + "{ background: transparent; border: 0.2ex solid white; border-radius: 3px;}");
    g_frame->setLayout(g_layout);

    return g_frame;
}

QVBoxLayout* HFUI::CreatePowerSuppliesStatusLayout() {
    // Global layout
    QVBoxLayout *layout = new QVBoxLayout;

    QGridLayout *grid_layout = new QGridLayout;
    QFont header_font = shared::FONT::MEDIUM_HEADER;
    QFont text_font = shared::FONT::SMALL_LABEL;

    const int indicator_diameter = 35;

    // Titles
    auto l = shared::utils::GetLabel("Marche", text_font);
    l->setMaximumHeight(20);
    grid_layout->addWidget(l, 0, 0, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Défaut", text_font), 0, 1, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Marche", text_font), 0, 3, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Défaut", text_font), 0, 4, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("PreAmpli", header_font), 1, 2, Qt::AlignHCenter);
    grid_layout->addWidget(shared::utils::GetLabel("Ampli", header_font), 2, 2, Qt::AlignHCenter);

    // Preamplis
    GenericDisplayButton* hf1_preampli_on = new GenericDisplayButton(this, indicator_diameter);
    hf1_preampli_on->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf1_preampli_on, 1, 0, Qt::AlignHCenter);
    GenericDisplayButton* hf1_preampli_defaut = new GenericDisplayButton(this, indicator_diameter);
    hf1_preampli_defaut->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf1_preampli_defaut, 1, 1, Qt::AlignHCenter);
    GenericDisplayButton* hf2_preampli_on = new GenericDisplayButton(this, indicator_diameter);
    hf2_preampli_on->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf2_preampli_on, 1, 3, Qt::AlignHCenter);
    GenericDisplayButton* hf2_preampli_defaut = new GenericDisplayButton(this, indicator_diameter);
    hf2_preampli_defaut->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf2_preampli_defaut, 1, 4, Qt::AlignHCenter);

    // Amplis
    GenericDisplayButton* hf1_ampli_on = new GenericDisplayButton(this, indicator_diameter);
    hf1_ampli_on->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf1_ampli_on, 2, 0, Qt::AlignHCenter);
    GenericDisplayButton* hf1_ampli_defaut = new GenericDisplayButton(this, indicator_diameter);
    hf1_ampli_defaut->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf1_ampli_defaut, 2, 1, Qt::AlignHCenter);
    GenericDisplayButton* hf2_ampli_on = new GenericDisplayButton(this, indicator_diameter);
    hf2_ampli_on->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf2_ampli_on, 2, 3, Qt::AlignHCenter);
    GenericDisplayButton* hf2_ampli_defaut = new GenericDisplayButton(this, indicator_diameter);
    hf2_ampli_defaut->SetupAsColorChanging(Qt::green, Qt::red);
    grid_layout->addWidget(hf2_ampli_defaut, 2, 4, Qt::AlignHCenter);

    QObject::connect(&powersupplies_interface_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Device_State,
                     this, [&, hf1_preampli_on, hf2_preampli_on, hf1_ampli_on, hf2_ampli_on](QString name, bool state) {
        qDebug() << "HFSTATE " << name << " " << state;
        if (name == "HF1 PreAmplis") {
            state ? hf1_preampli_on->On() : hf1_preampli_on->Off();
        } else if (name == "HF2 PreAmplis") {
            state ? hf2_preampli_on->On() : hf2_preampli_on->Off();
        } else if (name == "HF1 Amplis") {
            state ? hf1_ampli_on->On() : hf1_ampli_on->Off();
        } else if (name == "HF2 Amplis") {
            state ? hf2_ampli_on->On() : hf2_ampli_on->Off();
        }
    });

    QObject::connect(&powersupplies_interface_, &medicyc::cyclotron::HFPowerSuppliesInterface::SIGNAL_Device_Defaut,
                     this, [&, hf1_preampli_defaut, hf2_preampli_defaut, hf1_ampli_defaut, hf2_ampli_defaut](QString name, bool active) {
        qDebug() << "HFDEFAUT " << name << " " << active;
        if (name == "HF1 PreAmplis") {
            !active ? hf1_preampli_defaut->On() : hf1_preampli_defaut->Off();
        } else if (name == "HF2 PreAmplis") {
            !active ? hf2_preampli_defaut->On() : hf2_preampli_defaut->Off();
        } else if (name == "HF1 Amplis") {
            !active ? hf1_ampli_defaut->On() : hf1_ampli_defaut->Off();
        } else if (name == "HF2 Amplis") {
            !active ? hf2_ampli_defaut->On() : hf2_ampli_defaut->Off();
        }
    });

    layout->addLayout(grid_layout);

    return layout;
}

QHBoxLayout* HFUI::CreatePowerSuppliesCommandLayout() {
    const int pushbutton_width = 60;

    QHBoxLayout *layout = new QHBoxLayout;

    // preampli+amplis pushbuttons
    layout->addWidget(shared::utils::GetLabel("PréAmplis + Amplis", shared::FONT::MEDIUM_HEADER));
    auto pushButton_ampli_on =  shared::utils::GetPushButton("ON", shared::BUTTON::MEDIUM_HEIGHT);
    pushButton_ampli_on->setMinimumWidth(pushbutton_width);
    layout->addWidget(pushButton_ampli_on);
    QObject::connect(pushButton_ampli_on, &QPushButton::clicked, this, [&, this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "HF", "Allumer les préamplis et amplis de HF1 et HF2?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (reply == QMessageBox::Yes) powersupplies_interface_.Startup();
    });
    auto pushButton_ampli_off =  shared::utils::GetPushButton("OFF", shared::BUTTON::MEDIUM_HEIGHT);
    pushButton_ampli_off->setMinimumWidth(pushbutton_width);
    layout->addWidget(pushButton_ampli_off);
    QObject::connect(pushButton_ampli_off, &QPushButton::clicked, this, [&, this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "HF", "Eteindre les préamplis et amplis de HF1 et HF2?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (reply == QMessageBox::Yes) powersupplies_interface_.Shutdown();
    });
    // Other pushbuttons
    auto pushButton_detailed_view =  shared::utils::GetPushButton("Etat", shared::BUTTON::MEDIUM_HEIGHT);
    pushButton_detailed_view->setMinimumWidth(130);
    layout->addWidget(pushButton_detailed_view);
    QObject::connect(pushButton_detailed_view, &QPushButton::clicked, this, &HFUI::OpenDetailedPowerSupplyGui);

    return layout;
}


void HFUI::OpenDetailedPowerSupplyGui() {
    qDebug() << "HFUI::OpenDetailedPowerSupplyGui";
    detailed_ui_.ReadDeviceStatus();
    detailed_ui_.show();
}

void HFUI::OpenConfigurationGui() {
    qDebug() << "HFUI::OpenConfigurationGui";
    configuration_ui_.show();
}

} // ns

