#include "Touch1Gui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QSizePolicy>
#include <QSound>

#include "GenericDisplayButton.h"
#include "PoleLevelIndicator.h"
#include "EquipmentListGui.h"
#include "Utils.h"
#include "ClickableLabel.h"
#include "NumericInputDialog.h"
#include "HorisontalLevelBars.h"
#include "ClickableGeometricShape.h"
#include "DBus.h"
#include "Parameters.h"

namespace medicyc::cyclotroncontrolsystem::ui::touch1 {

Touch1Gui::Touch1Gui()
    : nsingle_repo_("_prod"),
      source_repo_("_prod"),
      settings_(QStandardPaths::locate(QStandardPaths::ConfigLocation, medicyc::cyclotroncontrolsystem::global::CONFIG_FILE, QStandardPaths::LocateFile),
                QSettings::IniFormat),
      fast_access_equipments_(GetQuickSelectionEquipments())
{
    setWindowTitle("CONTROLE ALIMENTATIONS");
    setFixedSize(1280, 800);
    CreateWidgets();
    FetchNSingles();
    FetchSourcePowerSupplies();
#ifdef RASPBERRY_PI
    SetupGPIO();
#endif
    QObject::connect(this, &Touch1Gui::SIGNAL_Equipment_Selected, this, &Touch1Gui::SIGNAL_Disable); // to reset ui components
    QObject::connect(this, &Touch1Gui::SIGNAL_Equipment_Selected, this, &Touch1Gui::ShowEquipment);

    // Check validity of connection once a while
    ping_timer_.setInterval(3000);
    ping_timer_.setSingleShot(false);
    QObject::connect(&ping_timer_, &QTimer::timeout, this, &Touch1Gui::Ping);
    ping_timer_.start();

    // Show first nsingle in quicklist on startup
    QTimer::singleShot(100, this, [&]() { if(!fast_access_equipments_.empty()) emit SIGNAL_Equipment_Selected(fast_access_equipments_.first()); });

    //    /* Let's color it a little to better realize the positioning: */
//        setStyleSheet("QWidget {"
//                      "border: 1px solid black;"
//                      "color: red"
//                      "}");
}

Touch1Gui::~Touch1Gui()
{
    qDebug() << "Touch1Gui::~Touch1Gui";
    for(auto ni : nsingle_interfaces_.keys()) {
        delete nsingle_interfaces_.value(ni);
    }
    for(auto ci : nsingle_controller_interfaces_.keys()) {
        delete nsingle_controller_interfaces_.value(ci);
    }
    for(auto p : sourcepowersupply_interfaces_.keys()) {
        delete sourcepowersupply_interfaces_.value(p);
    }

    nsingle_interfaces_.clear();
    nsingle_controller_interfaces_.clear();
    sourcepowersupply_interfaces_.clear();
    qDebug() << "Clean exit";
}

void Touch1Gui::ShowEquipment(QString name) {
    if (nsingle_controller_interfaces_.contains(name)) {
        ShowNSingle(name);
    } else if (sourcepowersupply_interfaces_.contains(name)) {
        ShowSourcePowerSupply(name);
    } else {
        qDebug() << "Touch1Gui::ShowEquipment No such equipment " << name;
    }
}

void Touch1Gui::ShowNSingle(QString name) {
    qDebug() << "Touch1Gui::ShowNSingle " << name;
    if (nsingle_controller_interfaces_.contains(name) && nsingle_interfaces_.contains(name)) {
        stacked_layout_->setCurrentWidget(nsingle_widget_);
        selected_equipment_ = name;
        auto ci = nsingle_controller_interfaces_[name];
        auto ni = nsingle_controller_interfaces_[name];
        if (ni->isValid() && ci->isValid()) {
            emit SIGNAL_Enable();
           } else {
            emit SIGNAL_Disable();
        }
        ci->QueryState();
        update();
    } else {
        selected_equipment_ = "none";
        emit SIGNAL_Disable();
        UserErrorPopup("NSingle introuvable: " + name);
    }
}

void Touch1Gui::ShowSourcePowerSupply(QString name) {
    qDebug() << "Touch1Gui::ShowSourcePowerSupply " << name;
    if (sourcepowersupply_interfaces_.contains(name)) {
        stacked_layout_->setCurrentWidget(sourcepowersupply_widget_);
        selected_equipment_ = name;
        auto p = sourcepowersupply_interfaces_[name];
        if (p->isValid()) {
            emit SIGNAL_Enable();
           } else {
            emit SIGNAL_Disable();
        }
//        p->QueryState(); // TODO ADD functionality
        update();
    } else {
        selected_equipment_ = "none";
        emit SIGNAL_Disable();
        UserErrorPopup("Alim source introuvable: " + name);
    }
}

void Touch1Gui::Ping() {
    bool dbus_ok = false;
    if (nsingle_controller_interfaces_.contains(selected_equipment_)) {
        dbus_ok = nsingle_controller_interfaces_[selected_equipment_]->isValid();
    } else if (sourcepowersupply_interfaces_.contains(selected_equipment_)) {
        dbus_ok = sourcepowersupply_interfaces_[selected_equipment_]->isValid();
    } else {
        dbus_ok = true;
        qWarning() << "Touch1Gui::Ping Unknown equipment: " << selected_equipment_;
        return;
    }

    if (dbus_ok) {
        emit SIGNAL_Enable();
    } else {
        emit SIGNAL_Disable();
    }
}

void Touch1Gui::FetchNSingles() {
    try {
        std::vector<QString> nsingles = nsingle_repo_.GetNSingles(GetSubSystems());
        for (auto& n : nsingles) {
            nsingle_addresses_.insert(n, nsingle_repo_.GetNSingleDbusAddress(n));
            ConnectNSingle(n);
        }
    } catch (std::exception& exc) {
        nsingle_addresses_.clear();
        UserErrorPopup(QString("Erreur lors de la récupération des données nsingle de la base de données: ") + exc.what());
    }
}

void Touch1Gui::FetchSourcePowerSupplies() {
    try {
        QStringList power_supplies = source_repo_.GetPowerSupplies();
        for (auto& p : power_supplies) {
            sourcepowersupply_configs_.insert(p, source_repo_.GetConfig(p));
            sourcepowersupply_addresses_.insert(p, source_repo_.GetDbusAddress(p));
            ConnectSourcePowerSupply(p);
        }
    } catch (std::exception& exc) {
        sourcepowersupply_addresses_.clear();
        UserErrorPopup(QString("Erreur lors de la récupération des données alim source de la base de données: ") + exc.what());
    }
}

#ifdef RASPBERRY_PI
void Touch1Gui::SetupGPIO() {
    QObject::connect(&gpio_, &raspberry::GPIOHandler::SIGNAL_INC, this, [&]() { emit SIGNAL_CommandInc(act_inc_steps_); });
    QObject::connect(&gpio_, &raspberry::GPIOHandler::SIGNAL_DEC, this, [&]() { emit SIGNAL_CommandDec(act_inc_steps_); });
    QObject::connect(&gpio_, &raspberry::GPIOHandler::SIGNAL_INC, this, &Touch1Gui::PlayBeep);
    QObject::connect(&gpio_, &raspberry::GPIOHandler::SIGNAL_DEC, this, &Touch1Gui::PlayBeep);
}
#endif

void Touch1Gui::CreateWidgets() {
    static bool widgets_created = false;
    if (widgets_created) {
        qWarning() << "Touch1Gui::CreateWidgets Widgets already created.";
        exit(-1);
    } else {
        widgets_created = true;
    }

    // Top row (title)
    QFrame *topFrame = SetupTopFrame();

    // Setup central Area
    QFrame *centralFrame = new QFrame;
    nsingle_widget_ = CreateNSingleWidget();
    sourcepowersupply_widget_ = CreateSourcePowerSupplyWidget();
    stacked_layout_ = new QStackedLayout;
    stacked_layout_->addWidget(nsingle_widget_);
    stacked_layout_->addWidget(sourcepowersupply_widget_);
    centralFrame->setLayout(stacked_layout_);

    // Setup side bar
    QFrame *sideFrame = SetupQuickSelectionBar();

    // Put everything on the grid layout
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(topFrame, 0, 0, 1, 1);
    gridLayout->addWidget(centralFrame, 1, 0, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(sideFrame, 0, 1, 2, 1);

    gridLayout->setRowStretch(0, 1);
    gridLayout->setRowStretch(1, 5);

    this->setLayout(gridLayout);
    update();
}

QWidget* Touch1Gui::CreateNSingleWidget() {
    // Setup central Area    
    QFrame *valuesFrame = new QFrame;
    valuesFrame->setObjectName("valuesFrame");
    QGridLayout *valuesLayout = SetupNSingleValuesGrid();
    valuesLayout->setContentsMargins(0, 0, 0, 0);
    valuesFrame->setLayout(valuesLayout);
    valuesFrame->setMinimumHeight(400);
    valuesFrame->setMinimumWidth(1000);
    valuesFrame->setMaximumWidth(1000);
    shared::utils::SetBorderColor(valuesFrame, QRgb(0x209fdf), 3);

    QFrame *statusFrame = new QFrame;
    statusFrame->setObjectName("statusFrame");
    QGridLayout* statusLayout = SetupNSingleStatusGrid();
    statusLayout->setContentsMargins(0, 0, 0, 0);
    statusFrame->setLayout(statusLayout);

    QHBoxLayout* commandLayout = SetupNSingleCommandGrid();

    // Put everything on the grid layout
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(valuesFrame, 0, 0, 1, 1, Qt::AlignHCenter);
    gridLayout->addWidget(statusFrame, 1, 0, 1, 1);
    gridLayout->addLayout(commandLayout, 2, 0, 1, 1);

    gridLayout->setRowStretch(0, 5);
    gridLayout->setRowStretch(1, 1);
    gridLayout->setRowStretch(2, 2);

    QWidget *w = new QWidget;
    w->setLayout(gridLayout);
    return w;
}

QWidget* Touch1Gui::CreateSourcePowerSupplyWidget() {
    // Setup central Area
    QFrame *valuesFrame = new QFrame;
    valuesFrame->setObjectName("valuesFrame");
    QGridLayout *valuesLayout = SetupSourcePowerSupplyValuesGrid();
    valuesLayout->setContentsMargins(0, 0, 0, 0);
    valuesFrame->setLayout(valuesLayout);
    valuesFrame->setMinimumHeight(400);
    valuesFrame->setMinimumWidth(1000);
    valuesFrame->setMaximumWidth(1000);
    shared::utils::SetBorderColor(valuesFrame, QRgb(0x209fdf), 3);

    QHBoxLayout* commandLayout = SetupSourcePowerSupplyCommandGrid();

    // Put everything on the grid layout
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setContentsMargins(0, 0, 0, 0);
    gridLayout->setSpacing(0);
    gridLayout->addWidget(valuesFrame, 0, 0, 1, 1, Qt::AlignHCenter);
    gridLayout->addLayout(commandLayout, 1, 0, 1, 1);

    gridLayout->setRowStretch(0, 5);
    gridLayout->setRowStretch(1, 2);

    QWidget *w = new QWidget;
    w->setLayout(gridLayout);
    return w;
}

QFrame* Touch1Gui::SetupTopFrame() {
    // Top row (title)
    QFrame *topFrame = new QFrame;
    topFrame->setObjectName("topFrame");
    QHBoxLayout* topLayout = new QHBoxLayout;
    topFrame->setLayout(topLayout);

    QPushButton *pushButton_exit = new QPushButton("X");
    pushButton_exit->setMaximumHeight(60);
    pushButton_exit->setMaximumWidth(60);
    pushButton_exit->setMinimumWidth(60);
    pushButton_exit->setMinimumHeight(60);
    QObject::connect(pushButton_exit, &QPushButton::clicked, this, [&]() { exit(1); });
    topLayout->addWidget(pushButton_exit, 0, Qt::AlignTop | Qt::AlignLeft);

    QLabel *l_title = new QLabel("");
    QObject::connect(this, &Touch1Gui::SIGNAL_Equipment_Selected, this, [&, l_title](QString name) { l_title->setText(name.replace('_', ' ')); });
    QFont title_font("Arial", 54);
    title_font.setBold(true);
    l_title->setFont(title_font);
    l_title->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    l_title->setMaximumHeight(150);
    l_title->setMinimumWidth(1050);
    topLayout->addWidget(l_title);

    return topFrame;
}

QHBoxLayout* Touch1Gui::SetupNSingleCommandGrid() {
    int minHeight = 100;
    QFont font("Arial", 15);
    QPushButton *startup =  shared::utils::GetPushButton("Démarrage\nAutomatique", font, minHeight);
    QPushButton *shutdown =  shared::utils::GetPushButton("Arrêt\nAutomatique", font, minHeight);
    QPushButton *on =  shared::utils::GetPushButton("Marche", font, minHeight);
    QPushButton *off =  shared::utils::GetPushButton("Arrêt", font, minHeight);
    QPushButton *reset =  shared::utils::GetPushButton("Reset", font, minHeight);
    QPushButton *polarity_change =  shared::utils::GetPushButton("Ch.Polarité", font, minHeight);

    QObject::connect(startup, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Démarrer l'alimentation")) {
            emit SIGNAL_CommandStartup();
        }}
    );

    QObject::connect(shutdown, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Arrêter l'alimentation")) {
            emit SIGNAL_CommandShutdown();
        }}
    );
    QObject::connect(on, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Mettre en marche l'alimentation")) {
            emit SIGNAL_CommandOn();
        }}
    );

    QObject::connect(off, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Couper l'alimentation")) {
            emit SIGNAL_CommandOff();
        }}
    );
    QObject::connect(reset, &QPushButton::clicked, this, &Touch1Gui::SIGNAL_CommandReset);
    QObject::connect(polarity_change, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Changer la polarité de l'alimentation")) {
            emit SIGNAL_CommandPolarityChange();
        }}
    );

    QObject::connect(this, &Touch1Gui::SIGNAL_Enable, this, [startup, shutdown, on, off, reset, polarity_change]() {
        startup->setEnabled(true);
        shutdown->setEnabled(true);
        on->setEnabled(true);
        off->setEnabled(true);
        reset->setEnabled(true);
        polarity_change->setEnabled(true);
    });
    QObject::connect(this, &Touch1Gui::SIGNAL_Disable, this, [startup, shutdown, on, off, reset, polarity_change]() {
        startup->setDisabled(true);
        shutdown->setDisabled(true);
        on->setDisabled(true);
        off->setDisabled(true);
        reset->setDisabled(true);
        polarity_change->setDisabled(true);
    });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(20);
    layout->addWidget(startup);
    layout->addWidget(shutdown);
    layout->addWidget(on);
    layout->addWidget(off);
    layout->addWidget(reset);
    layout->addWidget(polarity_change);

    return layout;
}

QGridLayout* Touch1Gui::SetupNSingleValuesGrid() {
    // Actual value
    ClickableLabel *l_value = new ClickableLabel("-");
    l_value->setFont(QFont("Ariel", 100));

    QObject::connect(this, &Touch1Gui::SIGNAL_NSingle_ValueChanged, this, [l_value, this](double value, bool polarity) {
        l_value->setText((polarity ? "+" : "-") + QString::number(value, 'f', 2));
        update();
    });

    ClickableLabel *l_setpoint = new ClickableLabel("Consigne -");
    l_setpoint->setFont(QFont("Ariel", 26));
    l_setpoint->setMaximumHeight(100);
    QObject::connect(l_value, &ClickableLabel::SIGNAL_Clicked, this, &Touch1Gui::DisplayKeyboard);
    QObject::connect(l_setpoint, &ClickableLabel::SIGNAL_Clicked, this, &Touch1Gui::DisplayKeyboard);
    QObject::connect(this, &Touch1Gui::SIGNAL_NSingle_SetPointChanged, this, [l_setpoint, this](double value, bool polarity) {
        l_setpoint->setText(QString("Consigne ") + (polarity ? "+" : "-") + QString::number(value, 'f', 2));
        update();
    });

    QObject::connect(this, &Touch1Gui::SIGNAL_Disable, this, [l_setpoint, l_value]() {
        l_setpoint->setText("-");
        l_value->setText("-");
    });

    // Inc/Dec buttons
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, 0x209fdf);
    gradient.setColorAt(1.0, 0x0E6692);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    QFrame *incDecFrame = new QFrame;
    ClickableGeometricShape *incButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 100, 100, 180);
    HorisontalLevelBars *incdec_steps = new HorisontalLevelBars(this, 100, 80, gradient, Qt::black, inc_steps_);
    ClickableGeometricShape *decButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 100, 100, 0);

    act_inc_steps_ = incdec_steps->ActiveLevel();
    QObject::connect(incdec_steps, &HorisontalLevelBars::SIGNAL_ActiveLevelChanged, this, [&](int steps) { act_inc_steps_ = steps; });
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { emit SIGNAL_CommandInc(act_inc_steps_); });
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { emit SIGNAL_CommandDec(act_inc_steps_); });
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, &Touch1Gui::PlayBeep);
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, &Touch1Gui::PlayBeep);
    QObject::connect(this, &Touch1Gui::SIGNAL_Equipment_Selected, incdec_steps, &HorisontalLevelBars::ResetLevel);

    QVBoxLayout *incDecLayout = new QVBoxLayout;
    incDecLayout->setContentsMargins(30, 10, 10, 10);
    incDecLayout->addWidget(incButton);
    incDecLayout->addSpacing(20);
    incDecLayout->addWidget(incdec_steps);
    incDecLayout->addSpacing(20);
    incDecLayout->addWidget(decButton);
    incDecFrame->setLayout(incDecLayout);

    QVBoxLayout *valuesLayout = new QVBoxLayout;
    valuesLayout->setContentsMargins(0, 0, 0, 0);
    valuesLayout->addSpacing(40);
    valuesLayout->addWidget(l_value);
    valuesLayout->addWidget(l_setpoint);
    l_value->setAlignment(Qt::AlignCenter);
    l_value->setMinimumWidth(830);
    l_value->setMaximumHeight(200);
    l_setpoint->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    l_setpoint->setMinimumHeight(90);
   // incDecLayout->addSpacing(20);

    // Put everything on a grid
    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->addWidget(incDecFrame, 0, 0, Qt::AlignLeft | Qt::AlignLeft);
    grid->addLayout(valuesLayout, 0, 1);

    return grid;
}

QGridLayout* Touch1Gui::SetupNSingleStatusGrid() {
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setHorizontalSpacing(5);
    gridLayout->setVerticalSpacing(5);

    QFont header_font = QFont("Arial", 12);
    int row = 0;
    int col = 0;
    gridLayout->addWidget(shared::utils::GetLabel("Connection", header_font), row, col++, Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Com", header_font), row, col++, Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Défaut", header_font), row, col++, Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Marche", header_font), row, col++, Qt::AlignHCenter | Qt::AlignVCenter);
    gridLayout->addWidget(shared::utils::GetLabel("OnTarget", header_font), row, col++, Qt::AlignHCenter | Qt::AlignVCenter);

    GenericDisplayButton *connected = new GenericDisplayButton(this, 50);
    connected->SetupAsColorChanging(Qt::green, Qt::red);
    PoleLevelIndicator *io_load = new PoleLevelIndicator(this, 15, 45, 3, 8);
    GenericDisplayButton *state = new GenericDisplayButton(this, 50);
    state->SetupAsColorChanging(Qt::green, Qt::red);
    GenericDisplayButton *switched_on = new GenericDisplayButton(this, 50);
    switched_on->SetupAsColorChanging(Qt::green, Qt::red);
    GenericDisplayButton *ontarget = new GenericDisplayButton(this, 50);
    ontarget->SetupAsCenterFilling(QRgb(0x209fdf));

    row++;
    col = 0;
    gridLayout->addWidget(connected, row, col++, Qt::AlignHCenter | Qt::AlignTop);
    gridLayout->addWidget(io_load, row, col++, Qt::AlignHCenter | Qt::AlignTop);
    gridLayout->addWidget(state, row, col++, Qt::AlignHCenter | Qt::AlignTop);
    gridLayout->addWidget(switched_on, row, col++, Qt::AlignHCenter | Qt::AlignTop);
    gridLayout->addWidget(ontarget, row, col++, Qt::AlignHCenter | Qt::AlignTop);

    QObject::connect(this, &Touch1Gui::SIGNAL_Connected, connected, &GenericDisplayButton::On);
    QObject::connect(this, &Touch1Gui::SIGNAL_Disconnected, connected, &GenericDisplayButton::Off);
    QObject::connect(this, &Touch1Gui::SIGNAL_IOLoad, io_load, &PoleLevelIndicator::SetLevel);
    QObject::connect(this, &Touch1Gui::SIGNAL_Error, state, &GenericDisplayButton::Off);
    QObject::connect(this, &Touch1Gui::SIGNAL_Ok, state, &GenericDisplayButton::On);
    QObject::connect(this, &Touch1Gui::SIGNAL_On, switched_on, &GenericDisplayButton::On);
    QObject::connect(this, &Touch1Gui::SIGNAL_Off, switched_on, &GenericDisplayButton::Off);
    QObject::connect(this, &Touch1Gui::SIGNAL_OnTarget, ontarget, &GenericDisplayButton::On);
    QObject::connect(this, &Touch1Gui::SIGNAL_OffTarget, ontarget, &GenericDisplayButton::Off);

    QObject::connect(this, &Touch1Gui::SIGNAL_Enable, this, [connected, io_load, state, switched_on, ontarget]() {
        connected->Enable();
        io_load->SetEnabled(true);
        state->Enable();
        switched_on->Enable();
        ontarget->Enable();
    });

    QObject::connect(this, &Touch1Gui::SIGNAL_Disable, this, [connected, io_load, state, switched_on, ontarget]() {
        connected->Disable();
        io_load->SetEnabled(false);
        state->Disable();
        switched_on->Disable();
        ontarget->Disable();
    });

    return gridLayout;
}

QHBoxLayout* Touch1Gui::SetupSourcePowerSupplyCommandGrid() {
    int minHeight = 100;
    QFont font("Arial", 15);
    QPushButton *startup =  shared::utils::GetPushButton("Démarrage\nAutomatique", font, minHeight);
    QPushButton *shutdown =  shared::utils::GetPushButton("Arrêt\nAutomatique", font, minHeight);
    QPushButton *on =  shared::utils::GetPushButton("Marche", font, minHeight);
    QPushButton *off =  shared::utils::GetPushButton("Arrêt", font, minHeight);

    QVBoxLayout *v_layout = new QVBoxLayout;
    v_layout->addWidget(shared::utils::GetLabel("Marche", font), Qt::AlignCenter);
    GenericDisplayButton *switched_on = new GenericDisplayButton(this, 60);
    switched_on->SetupAsColorChanging(Qt::green, Qt::red);
    v_layout->addWidget(switched_on, Qt::AlignCenter);
    v_layout->addSpacing(40);

    QObject::connect(startup, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Démarrer l'alimentation")) {
            emit SIGNAL_CommandStartup();
        }}
    );

    QObject::connect(shutdown, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Arrêter l'alimentation")) {
            emit SIGNAL_CommandShutdown();
        }}
    );
    QObject::connect(on, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Mettre en marche l'alimentation")) {
            emit SIGNAL_CommandOn();
        }}
    );
    QObject::connect(off, &QPushButton::clicked, this, [&]() {
        if (UserQuestionPopup("Couper l'alimentation")) {
            emit SIGNAL_CommandOff();
        }}
    );
    QObject::connect(this, &Touch1Gui::SIGNAL_On, switched_on, &GenericDisplayButton::On);
    QObject::connect(this, &Touch1Gui::SIGNAL_Off, switched_on, &GenericDisplayButton::Off);

    QObject::connect(this, &Touch1Gui::SIGNAL_Enable, this, [startup, shutdown, on, off, switched_on]() {
        startup->setEnabled(true);
        shutdown->setEnabled(true);
        on->setEnabled(true);
        off->setEnabled(true);
        switched_on->Enable();
    });
    QObject::connect(this, &Touch1Gui::SIGNAL_Disable, this, [startup, shutdown, on, off, switched_on]() {
        startup->setDisabled(true);
        shutdown->setDisabled(true);
        on->setDisabled(true);
        off->setDisabled(true);
        switched_on->Disable();
    });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(20);
    layout->addWidget(startup);
    layout->addWidget(shutdown);
    layout->addWidget(on);
    layout->addWidget(off);
    layout->addSpacing(40);
    layout->addLayout(v_layout);

    return layout;
}

QGridLayout* Touch1Gui::SetupSourcePowerSupplyValuesGrid() {
    // Voltage
    ClickableLabel *l_voltage = new ClickableLabel("-");
    l_voltage->setFont(QFont("Ariel", 100));
    QObject::connect(this, &Touch1Gui::SIGNAL_SourcePowerSupply_Voltage, this, [l_voltage, this](double value) {
        QString unit = (sourcepowersupply_configs_.contains(selected_equipment_) ? sourcepowersupply_configs_[selected_equipment_].voltage_unit() : "?");
        l_voltage->setText(QString::number(value, 'f', 2) + " " + unit);
        update();
    });

    ClickableLabel *l_current = new ClickableLabel("-");
    l_current->setFont(QFont("Ariel", 26));
    l_current->setMaximumHeight(100);
    QObject::connect(this, &Touch1Gui::SIGNAL_SourcePowerSupply_Current, this, [l_current, this](double value) {
        QString unit = (sourcepowersupply_configs_.contains(selected_equipment_) ? sourcepowersupply_configs_[selected_equipment_].current_unit() : "?");
        l_current->setText(QString::number(value, 'f', 2) + " " + unit);
        update();
    });

    QObject::connect(this, &Touch1Gui::SIGNAL_Disable, this, [l_voltage, l_current]() {
        l_voltage->setText("-");
        l_current->setText("-");
    });

    // Inc/Dec buttons
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 1));
    gradient.setColorAt(0.0, 0x209fdf);
    gradient.setColorAt(1.0, 0x0E6692);
    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    QFrame *incDecFrame = new QFrame;
    ClickableGeometricShape *incButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 100, 100, 180);
    ClickableGeometricShape *decButton = new ClickableGeometricShape(this, ClickableGeometricShape::GeometricShape::DOUBLEARROW, gradient, 100, 100, 0);
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { emit SIGNAL_CommandInc(1); });
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, [this]() { emit SIGNAL_CommandDec(1); });
    QObject::connect(incButton, &ClickableGeometricShape::SIGNAL_Clicked, this, &Touch1Gui::PlayBeep);
    QObject::connect(decButton, &ClickableGeometricShape::SIGNAL_Clicked, this, &Touch1Gui::PlayBeep);

    QVBoxLayout *incDecLayout = new QVBoxLayout;
    incDecLayout->setContentsMargins(30, 10, 10, 10);
    incDecLayout->addWidget(incButton);
    incDecLayout->addSpacing(20);
    incDecLayout->addWidget(decButton);
    incDecFrame->setLayout(incDecLayout);

    QVBoxLayout *valuesLayout = new QVBoxLayout;
    valuesLayout->setContentsMargins(0, 0, 0, 0);
    valuesLayout->addSpacing(40);
    valuesLayout->addWidget(l_voltage);
    valuesLayout->addWidget(l_current);
    l_voltage->setAlignment(Qt::AlignCenter);
    l_voltage->setMinimumWidth(830);
    l_voltage->setMaximumHeight(200);
    l_current->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    l_current->setMinimumHeight(90);
   // incDecLayout->addSpacing(20);

    // Put everything on a grid
    QGridLayout *grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->addWidget(incDecFrame, 0, 0, Qt::AlignLeft | Qt::AlignLeft);
    grid->addLayout(valuesLayout, 0, 1);

    return grid;
}

void Touch1Gui::DisplayKeyboard() {
    qDebug() << "Touch1Gui::DisplayKeyboard";
    NumericInputDialog *dialog = new NumericInputDialog(this);
    QObject::connect(dialog, &NumericInputDialog::SIGNAL_Value, this, &Touch1Gui::SIGNAL_CommandSetPoint);
    QObject::connect(dialog, &NumericInputDialog::accepted, dialog, &EquipmentListGui::deleteLater);
    QObject::connect(dialog, &NumericInputDialog::rejected, dialog, &EquipmentListGui::deleteLater);
    dialog->show();
}

void Touch1Gui::ConnectNSingle(QString nsingle) {
    qDebug() << "Touch1Gui::ConnectNSingle " << nsingle;
    if (nsingle_addresses_.contains(nsingle)) {
        if (!nsingle_controller_interfaces_.contains(nsingle)) {
            hw_nsingle::NSingleDbusAddress dbus_addresses = nsingle_addresses_[nsingle];
            nsingle_controller_interfaces_[nsingle] = new medicyc::cyclotron::NSingleControllerInterface(dbus_addresses.interface_address(), dbus_addresses.controller_object_name(),
                                                                                     medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
        }
        if (!nsingle_interfaces_.contains(nsingle)) {
            hw_nsingle::NSingleDbusAddress dbus_addresses = nsingle_addresses_[nsingle];
            nsingle_interfaces_[nsingle] = new medicyc::cyclotron::NSingleInterface(dbus_addresses.interface_address(), dbus_addresses.nsingle_object_name(),
                                                                medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
        }
        medicyc::cyclotron::NSingleControllerInterface *ci = nsingle_controller_interfaces_[nsingle];
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Disconnected, this, [this, nsingle]() { if(selected_equipment_ == nsingle) emit SIGNAL_Disconnected(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Connected, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_Connected(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareError, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_Error(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareOk, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_Ok(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_IOLoad, this, [this, nsingle](double load) { if(selected_equipment_ == nsingle)  emit SIGNAL_IOLoad(load); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_On(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_Off(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [this, nsingle](double physical_value, bool polarity) { if(selected_equipment_ == nsingle)  emit SIGNAL_NSingle_ValueChanged(physical_value, polarity); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DesValue, this, [this, nsingle](double physical_value, bool polarity) { if(selected_equipment_ == nsingle)  emit SIGNAL_NSingle_SetPointChanged(physical_value, polarity); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_OnTarget(); });
        QObject::connect(ci, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, this, [this, nsingle]() { if(selected_equipment_ == nsingle)  emit SIGNAL_OffTarget(); });

        medicyc::cyclotron::NSingleInterface *ni = nsingle_interfaces_[nsingle];

        QObject::connect(this, &Touch1Gui::SIGNAL_CommandStartup, this, [this, nsingle, ci](){ if (selected_equipment_ == nsingle) ci->Startup(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandShutdown, this, [this, nsingle, ci](){ if (selected_equipment_ == nsingle) ci->Shutdown(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandPolarityChange, this, [this, nsingle, ci](){ if (selected_equipment_ == nsingle) ci->ChangePolarity(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandSetPoint, this, [this, nsingle, ci](double value){ if (selected_equipment_ == nsingle) ci->UpdateDesiredValue(value); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandInc, this, [this, nsingle, ci](int steps){ if (selected_equipment_ == nsingle) ci->Increment(steps); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandDec, this, [this, nsingle, ci](int steps){ if (selected_equipment_ == nsingle) ci->Decrement(steps); });

        QObject::connect(this, &Touch1Gui::SIGNAL_CommandOn, this, [this, nsingle, ni](){ if (selected_equipment_ == nsingle) ni->On(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandOff, this, [this, nsingle, ni](){ if (selected_equipment_ == nsingle) ni->Off(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandReset, this, [this, nsingle, ni](){ if (selected_equipment_ == nsingle) ni->Reset(); });
    } else {
        qWarning() << "Touch1Gui::ConnectNSingle Not found " << nsingle;
    }
}

void Touch1Gui::ConnectSourcePowerSupply(QString power_supply) {
    qDebug() << "Touch1Gui::ConnectSourcePowerSupply " << power_supply;
    if (sourcepowersupply_addresses_.contains(power_supply)) {
        if (!sourcepowersupply_interfaces_.contains(power_supply)) {
            hw_source::PowerSupplyDbusAddress dbus_addresses = sourcepowersupply_addresses_[power_supply];
            sourcepowersupply_interfaces_[power_supply] = new medicyc::cyclotron::SourcePowerSupplyInterface(dbus_addresses.interface_address(), dbus_addresses.object_name(),
                                                                                                             medicyc::cyclotroncontrolsystem::global::GetDBusConnection(), this);
            qDebug() << "Touch1Gui::ConnectSourcePowerSupply " << power_supply << " DBUS " << (sourcepowersupply_interfaces_[power_supply]->isValid() ? "OK" : "ERROR");
        }
        medicyc::cyclotron::SourcePowerSupplyInterface *iface = sourcepowersupply_interfaces_[power_supply];
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandStartup, this, [this, power_supply, iface]() { if (selected_equipment_ == power_supply) iface->Startup(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandShutdown, this, [this, power_supply, iface]() { if (selected_equipment_ == power_supply) iface->Shutdown(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandOn, this, [this, power_supply, iface]() { if (selected_equipment_ == power_supply) iface->SwitchOn(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandOff, this, [this, power_supply, iface]() { if (selected_equipment_ == power_supply) iface->SwitchOff(); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandInc, this, [this, power_supply, iface](int steps) { if (selected_equipment_ == power_supply) iface->Increment(steps); });
        QObject::connect(this, &Touch1Gui::SIGNAL_CommandDec, this, [this, power_supply, iface](int steps) { if (selected_equipment_ == power_supply) iface->Decrement(steps); });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_On, this, [this, power_supply]() { if(selected_equipment_ == power_supply) emit SIGNAL_On(); });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Off, this, [this, power_supply]() { if(selected_equipment_ == power_supply) emit SIGNAL_Off(); });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Current, this, [this, power_supply](double value) { if(selected_equipment_ == power_supply) emit SIGNAL_SourcePowerSupply_Current(value) ; });
        QObject::connect(iface, &medicyc::cyclotron::SourcePowerSupplyInterface::SIGNAL_Voltage , this, [this, power_supply](double value) { if(selected_equipment_ == power_supply) emit SIGNAL_SourcePowerSupply_Voltage(value); });
    } else {
        qWarning() << "Touch1Gui::ConnectSourcePowerSupply Not found " << power_supply;
    }
}


QFrame* Touch1Gui::SetupQuickSelectionBar() {
    QFrame *frame = new QFrame;
    frame->setMaximumWidth(225);
    frame->setMinimumWidth(225);
    frame->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding);
    frame->setObjectName("sideFrame");
    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);
    frame->setLayout(sideLayout);

    QFont font("Arial", 15);
    for (QString& name : fast_access_equipments_) {
        QPushButton*  pushButton = shared::utils::GetPushButton(QString(name).replace("_"," "), font, 20);
        QObject::connect(pushButton, &QPushButton::clicked, this, [this, name] { emit SIGNAL_Equipment_Selected(name); });
        sideLayout->addWidget(pushButton);
    }

    QPushButton* pushButton_openEqList  = shared::utils::GetPushButton("Autres", font, 20);
    QObject::connect(pushButton_openEqList, &QPushButton::clicked, this, &Touch1Gui::OpenEquipmentListWidget);
    sideLayout->addWidget(pushButton_openEqList);
    return frame;
}

void Touch1Gui::OpenEquipmentListWidget() {
    QStringList equipment = (nsingle_addresses_.keys() + sourcepowersupply_addresses_.keys());
    std::sort(equipment.begin(), equipment.end(), std::less<QString>());
    EquipmentListGui *eq_list_gui = new EquipmentListGui(equipment);
    QObject::connect(eq_list_gui, &EquipmentListGui::SIGNAL_EquipmentSelected, this, &Touch1Gui::SIGNAL_Equipment_Selected);
    QObject::connect(eq_list_gui, &EquipmentListGui::SIGNAL_EquipmentSelected, eq_list_gui, &EquipmentListGui::deleteLater);
    QObject::connect(eq_list_gui, &EquipmentListGui::destroyed, eq_list_gui, &EquipmentListGui::deleteLater);
    eq_list_gui->show();
}

QStringList Touch1Gui::GetSubSystems() const {
    QStringList types = settings_.value("ui/touch1/sub_systems").toStringList();
    return types;
}

QStringList Touch1Gui::GetQuickSelectionEquipments() const {
    qDebug() << "Touch1Gui::GetQuickSelectionEq";
    QStringList names = settings_.value("ui/touch1/fast_access_buttons").toStringList();
    return names;
}

bool Touch1Gui::UserQuestionPopup(QString question) {
    QMessageBox msg;
    msg.setText(question + "?");
//    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msg.setStyleSheet("QLabel{min-width:500px; min-height:120px; font-size: 32px;} QPushButton{ width:250px; height:100px; font-size: 30px; }");

    bool reply(false);
    int ret = msg.exec();
    switch (ret) {
    case QMessageBox::Ok:
        reply = true;
        break;
    case QMessageBox::Cancel:
        reply = false;
        break;
    default:
        qWarning() << "Touch1Gui::UserQuestionPopup Run into default clause in switch.";
        reply = false;
        break;
    }
    return reply;
}

void Touch1Gui::UserErrorPopup(QString message) {
    QMessageBox msg;
    msg.setText(message);
//    msg.setIcon(QMessageBox::Question);
    msg.setStandardButtons(QMessageBox::Ok);
    msg.setStyleSheet("QLabel{min-width:400px; min-height:120px; font-size: 32px;} QPushButton{ width:400px; height:100px; font-size: 30px; }");
    msg.exec();
}

void Touch1Gui::PlayBeep() {
    qDebug() << "Play beep";
    QSound::play(":/sounds/beep2.wav");
}


}
