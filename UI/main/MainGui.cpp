#include "MainGui.h"

#include <QStandardPaths>
#include <QLabel>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QStackedWidget>
#include <QPrinter>
#include <QPrintDialog>
#include <QStackedWidget>
#include <QSound>

#include "ISubSystemUI.h"
#include "ConfigSelectionDialog.h"
#include "OmronSelectionDialog.h"
#include "Utils.h"
#include "PrintWidget.h"
#include "StateLabel.h"
#include "TimedState.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

MainGui::MainGui() :
    global_repo_("_prod"),
    io_load_(this),
    source_ui_(this),
    injection_nsingle_group_(this, "Injection", medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem::INJECTION),
    bp_group_(this),
    bc_nsingle_group_(this, "Bobines de correction", medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem::BOBINESDECORRECTION),
    ejection_nsingle_group_(this, "Ejection", medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem::EJECTION),
    hf_(this),
    vdf_nsingle_group_(this, "Voie de faisceau", medicyc::cyclotroncontrolsystem::global::Enumerations::SubSystem::VDF),
    cooling_ui_(this),
    vacuum_ui_(this),
    message_ui_(this)
{
    SetupUI();
    SetupGlobalStartupSM();
    SetupVoiceMessages();
}

MainGui::~MainGui()
{}

void MainGui::SetupVoiceMessages() {
    QObject::connect(&message_ui_, &MessageUI::SIGNAL_PlayErrorSound, &sound_messages_, &SoundMessages::AddErrorMessage);
    QObject::connect(&source_ui_, &SourceUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/SourceStartup.wav"); });
    QObject::connect(&bp_group_, &BobinePrincipaleUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/BPStartup.wav"); });
    QObject::connect(&injection_nsingle_group_, &NSingleGroupUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/InjectionStartup.wav"); });
    QObject::connect(&ejection_nsingle_group_, &NSingleGroupUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/EjectionStartup.wav"); });
    QObject::connect(&vdf_nsingle_group_, &NSingleGroupUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/VDFStartup.wav"); });
    QObject::connect(&bc_nsingle_group_, &NSingleGroupUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/BCStartup.wav"); });
    QObject::connect(&hf_, &HFUI::SIGNAL_StartupFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/HFStartup.wav"); });

    QObject::connect(&source_ui_, &SourceUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/SourceShutdown.wav"); });
    QObject::connect(&bp_group_, &BobinePrincipaleUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/BPShutdown.wav"); });
    QObject::connect(&injection_nsingle_group_, &NSingleGroupUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/InjectionShutdown.wav"); });
    QObject::connect(&ejection_nsingle_group_, &NSingleGroupUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/EjectionShutdown.wav"); });
    QObject::connect(&vdf_nsingle_group_, &NSingleGroupUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/VDFShutdown.wav"); });
    QObject::connect(&bc_nsingle_group_, &NSingleGroupUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/BCShutdown.wav"); });
    QObject::connect(&hf_, &HFUI::SIGNAL_ShutdownFinished, this, [&]() { sound_messages_.AddMessage(":/sounds/HFShutdown.wav"); });
    QObject::connect(&hf_, &HFUI::SIGNAL_FrequencyChanged, this, [&]() { sound_messages_.AddMessage(":/sounds/SingleTick.wav", false); });
}

void MainGui::SetupUI() {
    setWindowTitle("CYCLOTRON CONTROL SYSTEM");
    setFixedSize(2560, 1430);
    this->setObjectName("MainGui");
    QString stylesheet = QString("QWidget#") + this->objectName() + "{background: black;}";
    this->setStyleSheet(stylesheet);

    // top level gridlayout on which upperbar, sidebar and centralarea components are added
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(20);

    // Setup upper bar
    QFrame *upperBar = new QFrame;
    upperBar->setMaximumHeight(60);
    upperBar->setObjectName("upperbar");
    upperBar->setStyleSheet("QWidget#" + upperBar->objectName() + "{" +
            "background-color: black;" + "}");
    QFont title_font("Arial", 25);
    title_font.setBold(true);
    QLabel *upperTitle = shared::utils::GetLabel("MEDICYC 65 MeV CYCLOTRON", title_font, "White");//"#EF00AA");
    QPixmap image(":/images/logo_antoine_lacassagne_unicancer.png");
    QPixmap imageResized = image.scaledToHeight(60);//, 100, Qt::KeepAspectRatio);
    QLabel *upperImage = new QLabel();
    upperImage->setPixmap(imageResized);
    upperImage->setAttribute(Qt::WA_TranslucentBackground);
    QHBoxLayout *upperBarLayout = new QHBoxLayout;
    upperBarLayout->addWidget(upperTitle, 1, Qt::AlignLeft);
    upperBarLayout->addWidget(upperImage, 3, Qt::AlignRight);
    upperBar->setLayout(upperBarLayout);

    // Setup side bar
    QFrame *sideBar = new QFrame;
    sideBar->setObjectName("sidebar");
    sideBar->setMaximumWidth(210);
    shared::utils::SetGroupStyleSheet(sideBar);
    QVBoxLayout *sideBarLayout = new QVBoxLayout;
    sideBar->setLayout(sideBarLayout);
    // Commands acting on all subsystems
    SetupGlobalCommands(sideBarLayout);
    // Shows status of all subsystems
    auto global_status_frame = CreateGlobalStatusFrame();
    sideBarLayout->addWidget(global_status_frame);
    // Shows actual configuration and gives possibility to change
    auto config_frame = CreateConfigurationFrame();
    sideBarLayout->addWidget(config_frame);
    // Various commands that doesnt fit elsewhere (open external applications, show status microprocesses, etc)
    auto misc_frame = CreateMiscCommands();
    sideBarLayout->addWidget(misc_frame);

    // Setup central Area

    // Column 1
    QFrame *col1_row1_frame = new QFrame;
    col1_row1_frame->setObjectName("col1_row1_frame");
    shared::utils::SetGroupStyleSheet(col1_row1_frame);
    QVBoxLayout* col1_row1_layout = new QVBoxLayout;
    col1_row1_frame->setLayout(col1_row1_layout);
    col1_row1_layout->addWidget(&source_ui_);

    QFrame *col1_row2_frame = new QFrame;
    col1_row2_frame->setObjectName("col1_row2_frame");
    shared::utils::SetGroupStyleSheet(col1_row2_frame);
    QVBoxLayout* col1_row2_layout = new QVBoxLayout;
    col1_row2_frame->setLayout(col1_row2_layout);
    col1_row2_layout->addWidget(&injection_nsingle_group_);

    QFrame *col1_row3_frame = new QFrame;
    col1_row3_frame->setObjectName("col1_row3_frame");
    shared::utils::SetGroupStyleSheet(col1_row3_frame);
    QVBoxLayout* col1_row3_layout = new QVBoxLayout;
    col1_row3_frame->setLayout(col1_row3_layout);
    col1_row3_layout->addWidget(&cooling_ui_);

    QVBoxLayout* col1_Layout = new QVBoxLayout;
    col1_Layout->addWidget(col1_row1_frame);
    col1_Layout->addWidget(col1_row2_frame);
    col1_Layout->addWidget(col1_row3_frame);

    // Column 2
    QFrame *col2_row1_frame = new QFrame;
    col2_row1_frame->setObjectName("col2_row1_frame");
    shared::utils::SetGroupStyleSheet(col2_row1_frame);
    QVBoxLayout* col2_row1_layout = new QVBoxLayout;
    col2_row1_frame->setLayout(col2_row1_layout);
    col2_row1_layout->addWidget(&bp_group_);

    QFrame *col2_row2_frame = new QFrame;
    col2_row2_frame->setObjectName("col2_row2_frame");
    shared::utils::SetGroupStyleSheet(col2_row2_frame);
    QVBoxLayout* col2_row2_layout = new QVBoxLayout;
    col2_row2_frame->setLayout(col2_row2_layout);
    col2_row2_layout->addWidget(&bc_nsingle_group_);

    QFrame *col2_row3_frame = new QFrame;
    col2_row3_frame->setObjectName("col2_row3_frame");
    shared::utils::SetGroupStyleSheet(col2_row3_frame);
    QVBoxLayout* col2_row3_layout = new QVBoxLayout;
    col2_row3_frame->setLayout(col2_row3_layout);
    col2_row3_layout->addWidget(&ejection_nsingle_group_);

    QFrame *col2_row4_frame = new QFrame;
    col2_row4_frame->setObjectName("col2_row4_frame");
    shared::utils::SetGroupStyleSheet(col2_row4_frame);
    QVBoxLayout* col2_row4_layout = new QVBoxLayout;
    col2_row4_frame->setLayout(col2_row4_layout);
    col2_row4_frame->setMaximumHeight(150);
    col2_row4_layout->addWidget(&io_load_);

    QVBoxLayout* col2_Layout = new QVBoxLayout;
    col2_Layout->addWidget(col2_row1_frame);
    col2_Layout->addWidget(col2_row2_frame);
    col2_Layout->addWidget(col2_row3_frame);
    col2_Layout->addWidget(col2_row4_frame);

    // Column 3
    QFrame *col3_row1_frame = new QFrame;
    col3_row1_frame->setObjectName("col3_row1_frame");
    shared::utils::SetGroupStyleSheet(col3_row1_frame); // "#FFDB58"); // mustard yellow
    QVBoxLayout* col3_row1_layout = new QVBoxLayout;
    col3_row1_frame->setLayout(col3_row1_layout);
    col3_row1_layout->addWidget(&hf_);

    QVBoxLayout* col3_Layout = new QVBoxLayout;
    col3_Layout->addWidget(col3_row1_frame);

    // Column 4
    QFrame *col4_row1_frame = new QFrame;
    col4_row1_frame->setObjectName("col4_row1_frame");
    shared::utils::SetGroupStyleSheet(col4_row1_frame);
    QVBoxLayout* col4_row1_layout = new QVBoxLayout;
    col4_row1_frame->setLayout(col4_row1_layout);
    col4_row1_layout->addWidget(&vdf_nsingle_group_);

    QFrame *col4_row2_frame = new QFrame;
    col4_row2_frame->setObjectName("col4_row2_frame");
    shared::utils::SetGroupStyleSheet(col4_row2_frame);
    QVBoxLayout* col4_row2_layout = new QVBoxLayout;
    col4_row2_frame->setLayout(col4_row2_layout);
    col4_row2_layout->addWidget(&vacuum_ui_);

    QVBoxLayout* col4_Layout = new QVBoxLayout;
    col4_Layout->addWidget(col4_row1_frame);
    col4_Layout->addWidget(col4_row2_frame);

    // Col3-4
    QFrame *col34_row2_frame = new QFrame;
    col34_row2_frame->setObjectName("col34_row2_frame");
    shared::utils::SetGroupStyleSheet(col34_row2_frame, "#000000");
    QVBoxLayout* col34_row2_layout = new QVBoxLayout;
    col34_row2_frame->setLayout(col34_row2_layout);
    col34_row2_layout->addWidget(&message_ui_);

    QVBoxLayout* col34_Layout = new QVBoxLayout;
    col34_Layout->addWidget(col34_row2_frame);
    col34_row2_frame->setMaximumHeight(600);

    // Temporary until HF has been implemented properly
    hf_.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Put everything on the top level grid layout
    gridLayout->addWidget(upperBar, 0, 0, 1, 5);
    gridLayout->addWidget(sideBar, 1, 0, 2, 1);
    gridLayout->addLayout(col1_Layout, 1, 1, 2, 1);
    gridLayout->addLayout(col2_Layout, 1, 2, 2, 1);
    gridLayout->addLayout(col3_Layout, 1, 3, 1, 1);
    gridLayout->addLayout(col4_Layout, 1, 4, 1, 1);
    gridLayout->addLayout(col34_Layout, 2, 3, 1, 2);
}

void MainGui::SetupGlobalCommands(QBoxLayout* layout)  {
    QFrame *globalCommandsFrame = new QFrame;
    globalCommandsFrame->setLineWidth(2);
    QVBoxLayout *globalCommandsLayout = new QVBoxLayout;
    globalCommandsFrame->setLayout(globalCommandsLayout);

    QLabel* globalCommandsTitle = shared::utils::GetLabel("Commandes", shared::FONT::TITLE);
    globalCommandsTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    auto pushButton_startup =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::LARGE_HEIGHT*1.05);
    auto pushButton_shutdown =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::LARGE_HEIGHT*1.05);
    auto pushButton_interrupt =  shared::utils::GetPushButton("Interrompre", shared::BUTTON::LARGE_HEIGHT*1.05);
    auto globalCommandsLineSep = new QFrame;
    globalCommandsLineSep->setFixedHeight(3);
    globalCommandsLineSep->setFrameShadow(QFrame::Sunken);
    globalCommandsLineSep->setLineWidth(2);
    globalCommandsLineSep->setStyleSheet("background-color: cornflowerblue");
    globalCommandsLayout->addWidget(globalCommandsTitle);
    globalCommandsLayout->addWidget(globalCommandsLineSep);
    globalCommandsLayout->addSpacing(8);
    globalCommandsLayout->addWidget(pushButton_startup);
    globalCommandsLayout->addSpacing(8);
    globalCommandsLayout->addWidget(pushButton_shutdown);
    globalCommandsLayout->addSpacing(8);
    globalCommandsLayout->addWidget(pushButton_interrupt);

    QObject::connect(pushButton_startup, &QPushButton::clicked, this, [this]() {
        sound_messages_.AddMessage(":/sounds/AskStartupAll.wav");
    });
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, [this]() {
        sound_messages_.AddMessage(":/sounds/AskShutdownAll.wav");
    });

    QObject::connect(pushButton_startup, &QPushButton::clicked, this, [this]() {
        if (cooling_ui_.AllOn()) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Démarrer tous les systèmes?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
            if (reply == QMessageBox::Yes) emit SIGNAL_GlobalStartup();
        } else {
            QMessageBox::StandardButton reply = QMessageBox::warning(this, "MainGui", "Le système de refroidissement n'est pas complètement démarré. Voulez-vous toujours essayer de démarrer le cyclotron?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
            if (reply == QMessageBox::Yes) emit SIGNAL_GlobalStartup();
        }
    });
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, [this]() {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Arrêter tous les systèmes?", QMessageBox::Yes | QMessageBox::No);
          if (reply == QMessageBox::Yes) emit SIGNAL_GlobalShutdown();
    });
    QObject::connect(pushButton_interrupt, &QPushButton::clicked, this, [this]() {
        vdf_nsingle_group_.Interrupt();
        bc_nsingle_group_.Interrupt();
        ejection_nsingle_group_.Interrupt();
        injection_nsingle_group_.Interrupt();
        bp_group_.Interrupt();
        source_ui_.Interrupt();
        emit SIGNAL_GlobalInterrupt();
    });

    globalCommandsFrame->setAttribute(Qt::WA_TranslucentBackground);
    layout->addWidget(globalCommandsFrame);
}

QFrame* MainGui::CreateGlobalStatusFrame()  {
    auto GetSubSystemStatusLayout = [&] (ISubSystemUI* sub_system, QString title) -> QHBoxLayout* {
        StateLabel* label = new StateLabel(title);
        label->SetBlack();
        assert(QObject::connect(dynamic_cast<QObject*>(sub_system), SIGNAL(SIGNAL_State_Off()), label, SLOT(SetRed())));
        assert(QObject::connect(dynamic_cast<QObject*>(sub_system), SIGNAL(SIGNAL_State_Intermediate()), label, SLOT(SetYellow())));
        assert(QObject::connect(dynamic_cast<QObject*>(sub_system), SIGNAL(SIGNAL_State_Ready()), label, SLOT(SetGreen())));
        assert(QObject::connect(dynamic_cast<QObject*>(sub_system), SIGNAL(SIGNAL_State_Unknown()), label, SLOT(SetBlack())));
        QHBoxLayout *layout = new QHBoxLayout;
        layout->addWidget(label, Qt::AlignCenter);
        return layout;
    };

    QVector<QHBoxLayout*> subsystem_layouts;
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&bp_group_, "BP"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&vdf_nsingle_group_, "VDF"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&bc_nsingle_group_, "BCs"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&ejection_nsingle_group_, "Ejection"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&injection_nsingle_group_, "Injection"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&source_ui_, "Source"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&hf_, "HF"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&cooling_ui_, "Refroidissement"));
    subsystem_layouts.push_back(GetSubSystemStatusLayout(&vacuum_ui_, "Vide"));

    // Put everything together in a frame
    QFrame *globalStatusFrame = new QFrame;
    globalStatusFrame->setLineWidth(2);
    globalStatusFrame->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *globalStatusLayout = new QVBoxLayout;
    globalStatusFrame->setLayout(globalStatusLayout);
    QLabel* globalStatusTitle = shared::utils::GetLabel("Status", shared::FONT::TITLE);
    globalStatusTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    auto globalStatusLineSep = new QFrame;
    globalStatusLineSep->setFixedHeight(3);
    globalStatusLineSep->setFrameShadow(QFrame::Sunken);
    globalStatusLineSep->setLineWidth(2);
    globalStatusLineSep->setStyleSheet("background-color: cornflowerblue");
    globalStatusLayout->addWidget(globalStatusTitle);
    globalStatusLayout->addWidget(globalStatusLineSep);
    for (auto ss_l : subsystem_layouts) {
        globalStatusLayout->addSpacing(8);
        globalStatusLayout->addLayout(ss_l);
    }
    return globalStatusFrame;
}

QFrame* MainGui::CreateConfigurationFrame() {
    QFrame *configFrame = new QFrame;
    configFrame->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *configLayout = new QVBoxLayout;
    configFrame->setLayout(configLayout);

    QLabel* configTitle = shared::utils::GetLabel("Configuration", shared::FONT::TITLE);
    QFrame* configLineSep = new QFrame;
    configLineSep->setFixedHeight(3);
    configLineSep->setFrameShadow(QFrame::Sunken);
    configLineSep->setLineWidth(2);
    configLineSep->setStyleSheet("background-color: cornflowerblue");
    auto pushButtonLoadConfig =  shared::utils::GetPushButton("-", shared::BUTTON::LARGE_HEIGHT*1.05);
    configLayout->addWidget(configTitle);
    configLayout->addWidget(configLineSep);
    configLayout->addSpacing(8);
    configLayout->addWidget(pushButtonLoadConfig);
    configLayout->addSpacing(1);

    auto OpenConfigSelectionDialog = [&, pushButtonLoadConfig, this] () -> void {
        qDebug() << "MainGui::OpenConfigSelectionDialog";
        QMap<QDateTime, QStringList> configs = global_repo_.GetAvailableConfigs();
        ConfigSelectionDialog dialog(this, configs);
        const int response = dialog.exec();
        if (response == QDialog::Accepted) {
            auto selected_config = dialog.selected_config();
            injection_nsingle_group_.Configure(selected_config);
            vdf_nsingle_group_.Configure(selected_config);
            ejection_nsingle_group_.Configure(selected_config);
            bc_nsingle_group_.Configure(selected_config);
            source_ui_.Configure(selected_config);
            pushButtonLoadConfig->setText(selected_config);
            sound_messages_.AddMessage(":/sounds/NewConfiguration.wav");
        } else {
            qDebug() << "MainGui::OpenConfigSelectionDialog Cancelled";
        }
    };

    QObject::connect(pushButtonLoadConfig, &QPushButton::clicked, this, [this, OpenConfigSelectionDialog]() {
        try {
            OpenConfigSelectionDialog();
        } catch(std::exception& exc) {
            qDebug() << "MainGui::SetupConfigurationCommands Exception thrown: " << exc.what();
            message_ui_.ErrorMessage("Echec de l'ouverture de la fenêtre contextuelle de configuration");
        }
    });
    return configFrame;
}

QFrame* MainGui::CreateMiscCommands() {
    // Setup frame
    QFrame *miscFrame = new QFrame;
    miscFrame->setAttribute(Qt::WA_TranslucentBackground);
    QVBoxLayout *miscLayout = new QVBoxLayout;
    miscFrame->setLayout(miscLayout);
    QLabel* miscTitle = shared::utils::GetLabel("Misc", shared::FONT::TITLE);
    miscTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFrame* miscLineSep = new QFrame;
    miscLineSep->setFixedHeight(3);
    miscLineSep->setFrameShadow(QFrame::Sunken);
    miscLineSep->setLineWidth(2);
    miscLineSep->setStyleSheet("background-color: cornflowerblue");
    miscLayout->addWidget(miscTitle);
    miscLayout->addWidget(miscLineSep);

    // Add pushbuttons for actions

    QVector<QPushButton*> pushbuttons;

    auto pb_print = shared::utils::GetPushButton("Imprimer", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_print, &QPushButton::clicked, this, &MainGui::OpenPrintDialog);
    pushbuttons.push_back(pb_print);

    auto pb_processus = shared::utils::GetPushButton("Processus", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_processus, &QPushButton::clicked, &process_monitor_ui_, &ProcessMonitorUI::show);
    pb_processus->setStyleSheet(shared::utils::GetButtonStyleSheet(false));
    QObject::connect(&process_monitor_ui_, &ProcessMonitorUI::SIGNAL_RequiredProcessesRunning, this, [&, pb_processus](bool running) {
        pb_processus->setStyleSheet(shared::utils::GetButtonStyleSheet(running));
    });
    pushbuttons.push_back(pb_processus);

    auto pb_quartz = shared::utils::GetPushButton("Quartz", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_quartz, &QPushButton::clicked, this, [&]() { StartDetachedProcess("LaunchQuartzGui.sh", QStringList{}); });
    pushbuttons.push_back(pb_quartz);

    auto pb_beam = shared::utils::GetPushButton("Faisceau", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_beam, &QPushButton::clicked, this, [&]() { StartDetachedProcess("LaunchBeamlineGui.sh", QStringList{}); });
    pushbuttons.push_back(pb_beam);

    auto pb_grafana = shared::utils::GetPushButton("Grafana", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_grafana, &QPushButton::clicked, this, [&]() {
        StartDetachedProcess("LaunchGrafanaCyclotronGeneral.sh", QStringList{});
        StartDetachedProcess("LaunchGrafanaBalises.sh", QStringList{});
    });
    pushbuttons.push_back(pb_grafana);

    auto pb_nsingle = shared::utils::GetPushButton("NSingle", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_nsingle, &QPushButton::clicked, this, [&]() { StartDetachedProcess("LaunchNSingleGui.sh", QStringList{}); });
    pushbuttons.push_back(pb_nsingle);

    auto pb_omron = shared::utils::GetPushButton("Automates", shared::BUTTON::LARGE_HEIGHT*0.8);
    QObject::connect(pb_omron, &QPushButton::clicked, this, [&, this]() {
        OmronSelectionDialog dialog(this);
        const int response = dialog.exec();
        if (response == QDialog::Accepted) {
            QStringList args = dialog.Chosen();
            qDebug() << "Args " << args;
            for (auto &s: args) {
                StartDetachedProcess("LaunchOmronGui.sh", QStringList { s });
            }
        }
    });
    pushbuttons.push_back(pb_omron);

    for (auto pb : pushbuttons) {
        miscLayout->addSpacing(8);
        miscLayout->addWidget(pb);
    }

    return miscFrame;
}

void MainGui::OpenPrintDialog() {
    qDebug() << "MainGui::OpentPrintWidget";

    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print Document"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    std::vector<EquipmentGroupValues> group_values { vdf_nsingle_group_.GetGroupValues(),
                ejection_nsingle_group_.GetGroupValues(),
                injection_nsingle_group_.GetGroupValues(),
                bc_nsingle_group_.GetGroupValues(),
                bp_group_.GetGroupValues(),
                source_ui_.GetGroupValues() };
    PrintWidget print_widget(group_values);
    //status_gui->show();

    QPainter painter;
    painter.begin(&printer);
    double xscale = printer.pageLayout().paintRectPixels(printer.resolution()).width() / double(print_widget.width());
    double yscale = printer.pageLayout().paintRectPixels(printer.resolution()).height() / double(print_widget.height());
    double scale = qMin(xscale, yscale);
    painter.translate(printer.pageLayout().fullRectPixels(printer.resolution()).x() + printer.pageLayout().fullRectPixels(printer.resolution()).width()/2,
                      printer.pageLayout().fullRectPixels(printer.resolution()).y() + printer.pageLayout().fullRectPixels(printer.resolution()).height()/2);
    painter.scale(scale, scale);
    painter.translate(-print_widget.width()/2, -print_widget.height()/2);

    print_widget.render(&painter);
}

void MainGui::SetupGlobalStartupSM() {
    QState *sSuperState = new QState();
        QState *sIdle = new QState(sSuperState);
        QState *sStartup = new QState(sSuperState);
            // BCs should finished before starting up BP
            global::TimedState *sStartupFirstWave = new global::TimedState(sStartup, 300000, "Le démarrage des BC n'est pas encore terminé. La séquence de démarrage est interrompue", sIdle);
            global::TimedState *sStartupSecondWave = new global::TimedState(sStartup, 300000, "Timeout", sIdle);
            global::TimedState *sStartupInterrupt = new global::TimedState(sStartup, 4000, "", sStartupFirstWave);
         QState *sShutdown = new QState(sSuperState);
            // BP should finished before turning off the BCs
            global::TimedState *sShutdownFirstWave = new global::TimedState(sShutdown, 400000, "Le BP n'est toujours pas éteint, la séquence d'arrêt est interrompue", sIdle);
            global::TimedState *sShutdownSecondWave = new global::TimedState(sShutdown, 300000, "Timeout", sIdle);
            global::TimedState *sShutdownInterrupt = new global::TimedState(sShutdown, 4000, "", sShutdownFirstWave);

    // SuperState
    sSuperState->setInitialState(sIdle);
    sSuperState->addTransition(this, &MainGui::SIGNAL_GlobalStartup, sStartup);
    sSuperState->addTransition(this, &MainGui::SIGNAL_GlobalShutdown, sShutdown);
    sSuperState->addTransition(this, &MainGui::SIGNAL_GlobalInterrupt, sIdle);

    // Idle
    // no further actions

    // Startup
    sStartup->setInitialState(sStartupInterrupt);

    // StartupInterrupt
    QObject::connect(sStartupInterrupt, &QState::entered, this, [&]() {
        bc_nsingle_group_.Interrupt();
        vdf_nsingle_group_.Interrupt();
        ejection_nsingle_group_.Interrupt();
        injection_nsingle_group_.Interrupt();
         source_ui_.Interrupt();
    });

    // StartupFirstWave
    QObject::connect(sStartupFirstWave, &global::TimedState::SIGNAL_Timeout, &message_ui_, &MessageUI::ErrorMessage);
    sStartupFirstWave->addTransition(&bc_nsingle_group_, &NSingleGroupUI::SIGNAL_StartupFinished, sStartupSecondWave);
    QObject::connect(sStartupFirstWave, &QState::entered, this, [&]() {
        bc_nsingle_group_.Startup();
        vdf_nsingle_group_.Startup();
        ejection_nsingle_group_.Startup();
        injection_nsingle_group_.Startup();
    });

    // StartupSecondWave
    sStartupSecondWave->addTransition(&bp_group_, &BobinePrincipaleUI::SIGNAL_StartupFinished, sIdle);
    QObject::connect(sStartupSecondWave, &QState::entered, this, [&]() {
        source_ui_.Startup();
        bp_group_.Startup();
    });

    // Shutdown
    sShutdown->setInitialState(sShutdownInterrupt);

    // ShutdownInterrupt
    QObject::connect(sShutdownInterrupt, &QState::entered, this, [&]() {
        bc_nsingle_group_.Interrupt();
        vdf_nsingle_group_.Interrupt();
        ejection_nsingle_group_.Interrupt();
        injection_nsingle_group_.Interrupt();
        source_ui_.Interrupt();
    });

    // ShutdownFirstWave
    QObject::connect(sShutdownFirstWave, &global::TimedState::SIGNAL_Timeout, &message_ui_, &MessageUI::ErrorMessage);
    sShutdownFirstWave->addTransition(&bp_group_, &BobinePrincipaleUI::SIGNAL_ShutdownFinished, sShutdownSecondWave);
    QObject::connect(sShutdownFirstWave, &QState::entered, this, [&]() {
        bp_group_.Shutdown();
        vdf_nsingle_group_.Shutdown();
        injection_nsingle_group_.Shutdown();
        source_ui_.Shutdown();
    });

    // ShutdownSecondWave
    sShutdownSecondWave->addTransition(&bp_group_, &BobinePrincipaleUI::SIGNAL_ShutdownFinished, sIdle);
    QObject::connect(sShutdownSecondWave, &QState::entered, this, [&]() {
        ejection_nsingle_group_.Shutdown();
        bc_nsingle_group_.Shutdown();
    });

    PrintStateChanges(sSuperState, "SuperState");
    PrintStateChanges(sIdle, "Idle");
    PrintStateChanges(sStartup, "Startup");
    PrintStateChanges(sStartupInterrupt, "StartupInterrupt");
    PrintStateChanges(sStartupFirstWave, "StartupFirstWave");
    PrintStateChanges(sStartupSecondWave, "StartupSecondWave");
    PrintStateChanges(sShutdown, "Shutdown");
    PrintStateChanges(sShutdownInterrupt, "ShutdownInterrupt");
    PrintStateChanges(sShutdownFirstWave, "ShutdownFirstWave");
    PrintStateChanges(sShutdownSecondWave, "ShutdownSecondWave");

    sm_global_startup_.addState(sSuperState);
    sm_global_startup_.setInitialState(sSuperState);
    sm_global_startup_.start();
}

void MainGui::StartDetachedProcess(QString name, QStringList args) {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("LD_LIBRARY_PATH", "~/lib/");
    env.insert("PATH", "~/bin/");
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.setProcessEnvironment(env);
    process.setProgram(name);
    process.setArguments(args);

    if (process.startDetached()) {
        message_ui_.InfoMessage("Le processus " + name + " a été lancé");
    } else {
        message_ui_.ErrorMessage("Le processus " + name + " n'a pas pu démarrer: " + process.errorString());
    }
}

void MainGui::PrintStateChanges(QState *state, QString name) {
    QObject::connect(state, &QState::entered, this, [&, name]() { qDebug() << "MainGui " << QDateTime::currentDateTime() << " ->" << name; });
    QObject::connect(state, &QState::exited, this, [&, name]() { qDebug() << "MainGui " << QDateTime::currentDateTime() << " <-" << name; });
}

}
