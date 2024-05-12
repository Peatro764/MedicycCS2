#include "NSingleGroupUI.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QSound>
#include <QMessageBox>

#include "GenericDisplayButton.h"
#include "PoleLevelIndicator.h"
#include "Utils.h"
#include "NSingleDbusAddress.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

NSingleGroupUI::NSingleGroupUI(QWidget *parent, QString title, medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem sub_system) :
    QWidget(parent),
    title_(title),
    sub_system_(sub_system),
    sub_system_string_(medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystemToString(sub_system)),
    group_interface_(QString("medicyc.cyclotron.middlelayer.").append(sub_system_string_.toLower()),
                     QString("/").append(sub_system_string_.toUpper()),
                     medicyc::cyclotroncontrolsystem::global::GetDBusConnection()),
    repo_("_prod")
{
    // Relay group state to main gui
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_StartupFinished, this, &NSingleGroupUI::SIGNAL_StartupFinished);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_ShutdownFinished, this, &NSingleGroupUI::SIGNAL_ShutdownFinished);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_State_Off, this, &NSingleGroupUI::SIGNAL_State_Off);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_State_Intermediate, this, &NSingleGroupUI::SIGNAL_State_Intermediate);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_State_Ready, this, &NSingleGroupUI::SIGNAL_State_Ready);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_State_Unknown, this, &NSingleGroupUI::SIGNAL_State_Unknown);

    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel(title, shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    QLabel *l_config = shared::utils::GetLabel("-", shared::FONT::LARGE_LABEL);
    l_config->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_config, Qt::AlignRight | Qt::AlignVCenter);
    QObject::connect(&group_interface_, &middlelayer::NSingleGroupInterface::SIGNAL_Configuration, this, [&, l_config](QString name) { l_config->setText(name); });

    // Line separator
    auto line_1 = new QFrame;
    line_1->setFixedHeight(3);
    line_1->setFrameShadow(QFrame::Sunken);
    line_1->setLineWidth(2);
    line_1->setStyleSheet("background-color: cornflowerblue");

    // Central rows (nsingles)
    grid_layout_ = SetupGridLayout();

    // Bottom rows (pushbuttons)
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    QPushButton *pushButton_startup =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::MEDIUM_HEIGHT);
    QPushButton *pushButton_shutdown =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::MEDIUM_HEIGHT);
    QPushButton *pushButton_interrupt =  shared::utils::GetPushButton("Interrompre", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_startup, &QPushButton::clicked, this, &NSingleGroupUI::StartupAfterConfirmation);
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, &NSingleGroupUI::ShutdownAfterConfirmation);
    QObject::connect(pushButton_interrupt, &QPushButton::clicked, this, &NSingleGroupUI::Interrupt);

    bottom_layout->addWidget(pushButton_startup);
    bottom_layout->addWidget(pushButton_shutdown);
    bottom_layout->addWidget(pushButton_interrupt);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addLayout(grid_layout_);
    v_layout->addLayout(bottom_layout);

    AddSubSystemNSingles(sub_system_string_);

    // Check validity of dbus connection once in a while
    ping_timer_.setInterval(3000);
    ping_timer_.setSingleShot(false);
    QObject::connect(&ping_timer_, &QTimer::timeout, this, &NSingleGroupUI::Ping);
    ping_timer_.start();

    // Ask for the current configuration
    group_interface_.QueryConfiguration();
}

NSingleGroupUI::~NSingleGroupUI() {
    delete grid_layout_;
    for (auto &e : nsingle_controllers_.toStdMap())  {
        delete e.second;
    }
    for (auto &e : nsingle_uielements_.toStdMap())  {
        delete e.second;
    }
    // TODO delete all elements in the UIElement struct
}

QGridLayout* NSingleGroupUI::SetupGridLayout() {
    qDebug() << "NSingleGroupUI::SetupGridLayout";
    QGridLayout *gridLayout = new QGridLayout;
    QFont header_font = shared::FONT::SMALL_HEADER;
    int col = 0;
    gridLayout->addWidget(shared::utils::GetLabel("Démarrer", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Arrêter", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Equipement", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Connexion", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Com", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Défaut", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Allumée", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Config [A]", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Consigné [A]", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Valeur [A]", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Ontarget", header_font), gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridlayout_active_row_++;
    return gridLayout;
}

void NSingleGroupUI::Ping() {
    for (auto name : nsingle_controllers_.keys()) {
        auto u = nsingle_uielements_.value(name);
        auto c = nsingle_controllers_.value(name);
        ShowNSingle(u, u->available && c->isValid());
    }
}

void NSingleGroupUI::StartupAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", QString("Démarrer ") + title_.toLower() + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { group_interface_.Startup(); }
}

void NSingleGroupUI::ShutdownAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", QString("Arrêter ") + title_.toLower() + "?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { group_interface_.Shutdown(); }
}

void NSingleGroupUI::Startup() {
   group_interface_.Startup();
}

void NSingleGroupUI::Shutdown() {
   group_interface_.Shutdown();
}

void NSingleGroupUI::Interrupt() {
    group_interface_.Interrupt();
}

void NSingleGroupUI::Configure(QString name) {
    qDebug() << "NSingleGroupUI::Configure " << name;
    group_interface_.Configure(name);
}

void NSingleGroupUI::AddSubSystemNSingles(QString sub_system) {
    try {
        QMap<QString, bool> nsingles = repo_.GetAllNSingles(sub_system);
        for (auto& n : nsingles.keys()) {
            AddNSingle(n, repo_.GetNSingleShortName(n), repo_.GetNSingleDbusAddress(n), nsingles.value(n));
        }
    } catch(std::exception& exc) {
        qWarning() << "NSingleGroupUI::AddSubSystemNSingles Exception caught: " << exc.what();
    }
}

void NSingleGroupUI::AddNSingle(QString full_name, QString short_name, hw_nsingle::NSingleDbusAddress nsingle_address, bool available) {
    qDebug() << "NSingleGroupUI::AddNSingle " << full_name;
    NSingleUIElements* ui_elements = CreateNSingleUIElements(short_name, available);
    medicyc::cyclotron::NSingleControllerInterface* controller = new medicyc::cyclotron::NSingleControllerInterface(nsingle_address.interface_address(), nsingle_address.controller_object_name(),
                                                                                                                    medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
    PlaceOnGridLayout(ui_elements);
    if (available) ConnectNSingleUIElements(ui_elements, controller);
    nsingle_uielements_[full_name] = ui_elements;
    nsingle_controllers_[full_name] = controller;
    ShowNSingle(ui_elements, ui_elements->available && controller->isValid());
}

EquipmentGroupValues NSingleGroupUI::GetGroupValues() const {
    EquipmentGroupValues group_values(sub_system_string_);
    for (auto k : nsingle_uielements_.keys()) {
        NSingleUIElements *e = nsingle_uielements_.value(k);
        EquipmentValues eq_values(e->switched_on->text(),
                               e->commanded_value->text(),
                               e->actual_value->text());
        group_values.AddEquipment(k, eq_values);
    }
    return group_values;
}

void NSingleGroupUI::ShowNSingle(QString name, bool flag) {
    if (nsingle_uielements_.contains(name)) {
        ShowNSingle(nsingle_uielements_.value(name), flag);
    } else {
        qWarning() << "NSingleGroupUI::ShowNsingle No such nsingle " << name;
    }
}

void NSingleGroupUI::ShowNSingle(NSingleUIElements *e, bool flag) {
    flag ? e->connected->Enable() : e->connected->Disable();
    e->io_load->SetEnabled(flag);
    flag ? e->switched_on->Enable() : e->switched_on->Disable();
    e->startup->setEnabled(flag);
    e->config->setEnabled(flag);
    e->commanded_value->setEnabled(flag);
    flag ? e->state->Enable() : e->state->Disable();
    e->shutdown->setEnabled(flag);
    flag ? e->ontarget->Enable() : e->ontarget->Disable();
    e->actual_value->setEnabled(flag);
}

NSingleUIElements* NSingleGroupUI::CreateNSingleUIElements(QString short_name, bool available) {
    qDebug() << "NSingleGroupUI::CreateNSingleUIElements " << short_name;
    NSingleUIElements* ui_element = new NSingleUIElements;
    ui_element->available = available;
    ui_element->startup  = shared::utils::GetPushButton("D", shared::BUTTON::SMALL_HEIGHT);
    ui_element->shutdown = shared::utils::GetPushButton("A", shared::BUTTON::SMALL_HEIGHT);
    ui_element->startup->setMaximumWidth(35);
    ui_element->shutdown->setMaximumWidth(35);
    ui_element->startup->setMinimumWidth(35);
    ui_element->shutdown->setMinimumWidth(35);
    ui_element->name = shared::utils::GetLabel(short_name, shared::FONT::MEDIUM_LABEL);
    ui_element->name->setMinimumWidth(100);
    ui_element->connected = new GenericDisplayButton(this, 25);
    ui_element->connected->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->io_load = new PoleLevelIndicator(this, 10, 25, 3, 8);
    ui_element->state = new GenericDisplayButton(this, 25);
    ui_element->state->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->switched_on = new GenericDisplayButton(this, 25);
    ui_element->switched_on->SetupAsColorChanging(Qt::green, Qt::red);
    ui_element->config = shared::utils::GetLabel("-", shared::FONT::SMALL_LABEL);
    ui_element->commanded_value = shared::utils::GetLabel("-", shared::FONT::SMALL_LABEL);
    ui_element->actual_value = shared::utils::GetLabel("-", shared::FONT::LARGE_LABEL);
    ui_element->actual_value->setMinimumWidth(100);
    ui_element->ontarget = new GenericDisplayButton(this, 25);
    ui_element->ontarget->SetupAsCenterFilling(QRgb(0x209fdf));
    return ui_element;
}

void NSingleGroupUI::PlaceOnGridLayout(NSingleUIElements* elements) {
    qDebug() << "NSingleGroupUI::PlaceOnGridLayout " << elements->name;
    int col = 0;
    grid_layout_->addWidget(elements->startup, gridlayout_active_row_, col++);
    grid_layout_->addWidget(elements->shutdown, gridlayout_active_row_, col++);
    grid_layout_->addItem(new QSpacerItem(2, 2), gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->name, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addItem(new QSpacerItem(2, 2), gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->connected, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->io_load, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->state, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->switched_on, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->config, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->commanded_value, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->actual_value, gridlayout_active_row_, col++, Qt::AlignHCenter);
    grid_layout_->addWidget(elements->ontarget, gridlayout_active_row_, col++, Qt::AlignHCenter);
    gridlayout_active_row_++;
}

void NSingleGroupUI::ConnectNSingleUIElements(NSingleUIElements* uielements, medicyc::cyclotron::NSingleControllerInterface* controller) {
    qDebug() << "NSingleGroupUI::ConnectNSingleUIElements " << uielements->name;
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_IOLoad , this, [uielements] (double value) { uielements->io_load->SetLevel(value); });
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Disconnected, this, [uielements] () { uielements->connected->Off(); });
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Connected, this, [uielements]() { uielements->connected->On();});
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, uielements->switched_on, &GenericDisplayButton::Off);
    // Too annoying during shutdown
//    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_TransitionToOff, this, [&]() { QSound::play(":/sounds/equipmentoff.wav"); });

    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, uielements->switched_on, &GenericDisplayButton::On);
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, uielements->ontarget, &GenericDisplayButton::On);
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, uielements->ontarget, &GenericDisplayButton::Off);
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [uielements](double value, bool polarity) {
        uielements->actual_value->setText((polarity ? "+" : "-") + QString::number(value, 'f', 2));
    });
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DesValue, this, [uielements](double value, bool polarity) {
        uielements->commanded_value->setText((polarity ? "+" : "-") + QString::number(value, 'f', 2));
    });
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Configuration, this, [this, uielements, controller](double value, bool sign) {
        uielements->config->setText((sign ? "+" : "-") + QString::number(value, 'f', 2));
        ShowNSingle(uielements, uielements->available && controller->isValid());
    });
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareError, this, [uielements]() { uielements->state->Off();});
    QObject::connect(controller, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareOk, this, [uielements]() { uielements->state->On();});

    QObject::connect(uielements->startup, &QPushButton::clicked, controller, &medicyc::cyclotron::NSingleControllerInterface::Startup);
    QObject::connect(uielements->shutdown, &QPushButton::clicked, controller, &medicyc::cyclotron::NSingleControllerInterface::Shutdown);

    controller->QueryState();
}

}
