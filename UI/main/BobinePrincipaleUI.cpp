#include "BobinePrincipaleUI.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QSound>

#include "GenericDisplayButton.h"
#include "PoleLevelIndicator.h"
#include "Style.h"
#include "Utils.h"
#include "DBus.h"

namespace medicyc::cyclotroncontrolsystem::ui::main {

BobinePrincipaleUI::BobinePrincipaleUI(QWidget *parent) :
    QWidget(parent),
    controller_(medicyc::cyclotron::NSingleControllerInterface(QString("medicyc.cyclotron.nsingle.Bobine_Principale"), "/Controller",
                                      medicyc::cyclotroncontrolsystem::global::GetDBusConnection())),
    interface_(middlelayer::BobinePrincipaleCyclerInterface(QString("medicyc.cyclotron.middlelayer.bobine_principale_cycler"), QString("/BobinePrincipaleCycler"), medicyc::cyclotroncontrolsystem::global::GetDBusConnection()))
{
    // Relay group state to main gui
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_StartupFinished, this, &BobinePrincipaleUI::SIGNAL_StartupFinished);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_ShutdownFinished, this, &BobinePrincipaleUI::SIGNAL_ShutdownFinished);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_State_Off, this, &BobinePrincipaleUI::SIGNAL_State_Off);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_State_Intermediate, this, &BobinePrincipaleUI::SIGNAL_State_Intermediate);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_State_Ready, this, &BobinePrincipaleUI::SIGNAL_State_Ready);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_State_Unknown, this, &BobinePrincipaleUI::SIGNAL_State_Unknown);

    // Top row (titles)
    QHBoxLayout *top_layout = new QHBoxLayout;
    QLabel *l_title = shared::utils::GetLabel("Bobine principale", shared::FONT::TITLE);
    l_title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    top_layout->addWidget(l_title, Qt::AlignLeft | Qt::AlignVCenter);

    // Line separator
    auto line_1 = new QFrame;
    line_1->setFixedHeight(3);
    line_1->setFrameShadow(QFrame::Sunken);
    line_1->setLineWidth(2);
    line_1->setStyleSheet("background-color: cornflowerblue");

    // Center row
    QHBoxLayout *commandLayout = SetupCommandBar();
    SetupTimeseriesPlot();
    timeseries_plot_.setMinimumHeight(270);

    // Bottom row
    QHBoxLayout *bottom_layout = new QHBoxLayout;
    QGridLayout* statusBarLayout = SetupStatusBar();
    bottom_layout->addLayout(statusBarLayout, Qt::AlignRight);

    // Setup all rows
    QVBoxLayout *v_layout = new QVBoxLayout(this);
    v_layout->addLayout(top_layout);
    v_layout->addWidget(&timeseries_plot_);
    v_layout->addSpacing(10);
    v_layout->addLayout(bottom_layout);
    v_layout->addSpacing(10);
    v_layout->addLayout(commandLayout);

    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_CyclageStarted, this, &BobinePrincipaleUI::CyclageStarted);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_CyclageFinished, this, &BobinePrincipaleUI::CyclageFinished);
    QObject::connect(&interface_, &middlelayer::BobinePrincipaleCyclerInterface::SIGNAL_CyclageInterrupted, this, &BobinePrincipaleUI::CyclageInterrupted);

    SetupNSingleController();

    // Check validity of connection once a while
    ping_timer_.setInterval(3000);
    ping_timer_.setSingleShot(false);
    QObject::connect(&ping_timer_, &QTimer::timeout, this, &BobinePrincipaleUI::Ping);
    ping_timer_.start();
}

BobinePrincipaleUI::~BobinePrincipaleUI() {
    delete connected_;
    delete io_load_;
    delete switched_on_;
    delete state_;
    delete ontarget_;
    delete name_;
    delete commanded_value_;
    delete actual_value_;
}

void BobinePrincipaleUI::StartupAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Démarrer la bobine principale?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { interface_.Startup(); }
}

void BobinePrincipaleUI::ShutdownAfterConfirmation() {
    QMessageBox::StandardButton reply = QMessageBox::question(this, "MainGui", "Arrêter la bobine principale?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) { interface_.Shutdown(); }
}

void BobinePrincipaleUI::Configure(QString name) {
    (void)name;
    // BP has no configuration
}

void BobinePrincipaleUI::Startup() {
   interface_.Startup();
}

void BobinePrincipaleUI::Shutdown() {
   interface_.Shutdown();
}

void BobinePrincipaleUI::Interrupt() {
    interface_.Interrupt();
}

EquipmentGroupValues BobinePrincipaleUI::GetGroupValues() const {
    EquipmentGroupValues group_values("BOBINE PRINCIPALE");
    EquipmentValues eq_value(switched_on_->text(),
                             commanded_value_->text(),
                             actual_value_->text());
    group_values.AddEquipment("Bobine Principale", eq_value);
    return group_values;
}

QHBoxLayout* BobinePrincipaleUI::SetupCommandBar() {
    QHBoxLayout *layout = new QHBoxLayout;
    auto pushButton_startup =  shared::utils::GetPushButton("Démarrer", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_shutdown =  shared::utils::GetPushButton("Arrêter", shared::BUTTON::MEDIUM_HEIGHT);
    auto pushButton_interrupt =  shared::utils::GetPushButton("Interrompre", shared::BUTTON::MEDIUM_HEIGHT);
    QObject::connect(pushButton_startup, &QPushButton::clicked, this, &BobinePrincipaleUI::StartupAfterConfirmation);
    QObject::connect(pushButton_shutdown, &QPushButton::clicked, this, &BobinePrincipaleUI::ShutdownAfterConfirmation);
    QObject::connect(pushButton_interrupt, &QPushButton::clicked, this, &BobinePrincipaleUI::Interrupt);

    layout->addWidget(pushButton_startup);
    layout->addWidget(pushButton_shutdown);
    layout->addWidget(pushButton_interrupt);
    return layout;
}

void BobinePrincipaleUI::SetupTimeseriesPlot() {
    timeseries_plot_.setLocale(QLocale(QLocale::French, QLocale::Country::France));
    timeseries_plot_.addGraph();
    timeseries_plot_.yAxis->setRange(0.0, 1300);
    SetTimeSeriesColors(0x209fdf);

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss");
    timeseries_plot_.xAxis->setTicker(dateTicker);
    timeseries_plot_.xAxis->setTickLabelColor(Qt::white);
    timeseries_plot_.xAxis->setBasePen(QPen(Qt::white));
    timeseries_plot_.xAxis->setLabelColor(Qt::white);
    timeseries_plot_.xAxis->setTickPen(QPen(Qt::white));
    timeseries_plot_.xAxis->setSubTickPen(QPen(Qt::white));
    timeseries_plot_.yAxis->setTickLabelColor(Qt::white);
    timeseries_plot_.yAxis->setBasePen(QPen(Qt::white));
    timeseries_plot_.yAxis->setLabelColor(Qt::white);
    timeseries_plot_.yAxis->setTickPen(QPen(Qt::white));
    timeseries_plot_.yAxis->setSubTickPen(QPen(Qt::white));

//    timeseries_plot_.setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    timeseries_plot_.setBackground(QBrush(Qt::transparent));
    timeseries_plot_.setStyleSheet("background-color: transparent;");
    timeseries_plot_.replot();
}

QGridLayout* BobinePrincipaleUI::SetupStatusBar() {
    qDebug() << "BobinePrincipaleUI::SetupStatusBar";
    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->setHorizontalSpacing(5);
    gridLayout->setVerticalSpacing(5);

    QFont header_font = shared::FONT::MEDIUM_HEADER;
    int row = 0;
    int col = 0;
    gridLayout->addWidget(shared::utils::GetLabel("Connexion", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Com", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Défaut", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Allumée", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Consigné", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Valeur", header_font), row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(shared::utils::GetLabel("Ontarget", header_font), row, col++, Qt::AlignHCenter);

    connected_ = new GenericDisplayButton(this, 50);
    connected_->SetupAsColorChanging(Qt::green, Qt::red);
    io_load_ = new PoleLevelIndicator(this, 15, 45, 3, 8);
    state_ = new GenericDisplayButton(this, 50);
    state_->SetupAsColorChanging(Qt::green, Qt::red);
    switched_on_ = new GenericDisplayButton(this, 50);
    switched_on_->SetupAsColorChanging(Qt::green, Qt::red);
    commanded_value_ = shared::utils::GetLabel("-", shared::FONT::MEDIUM_LABEL);
    actual_value_ = shared::utils::GetLabel("-", shared::FONT::XLARGE_LABEL);
    ontarget_ = new GenericDisplayButton(this, 50);
    ontarget_->SetupAsCenterFilling(QRgb(0x209fdf));

    row++;
    col = 0;
    gridLayout->addWidget(connected_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(io_load_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(state_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(switched_on_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(commanded_value_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(actual_value_, row, col++, Qt::AlignHCenter);
    gridLayout->addWidget(ontarget_, row, col++, Qt::AlignHCenter);

    return gridLayout;
}

void BobinePrincipaleUI::SetupNSingleController() {
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_IOLoad , this, [this](double value) { io_load_->SetLevel(value); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Disconnected, this, [this] () { connected_->Off(); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Connected, this, [this]() { connected_->On(); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_Off, switched_on_, &GenericDisplayButton::Off);
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_On, switched_on_, &GenericDisplayButton::On);
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OnTarget, ontarget_, &GenericDisplayButton::On);
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_OffTarget, ontarget_, &GenericDisplayButton::Off);
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [&](double value) { UpdateTimeSeries(value); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_ActValue, this, [this](double value) { actual_value_->setText(QString::number(value, 'f', 2)); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_DesValue, this, [this](double value) { commanded_value_->setText(QString::number(value, 'f', 2)); });
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareError, this, [this]() { state_->Off();});
    QObject::connect(&controller_, &medicyc::cyclotron::NSingleControllerInterface::SIGNAL_HardwareOk, this, [this]() { state_->On();});
    controller_.QueryState();
}

void BobinePrincipaleUI::Ping() {
    Show(controller_.isValid());
}

void BobinePrincipaleUI::ShutdownStarted() {
    SetTimeSeriesColors(0x209fdf);
}

void BobinePrincipaleUI::CyclageStarted() {
    SetTimeSeriesColors(0x209fdf);
}

void BobinePrincipaleUI::CyclageFinished() {
    SetTimeSeriesColors(0x25d52a);
}

void BobinePrincipaleUI::CyclageInterrupted() {
    SetTimeSeriesColors(0xd83535);
}

void BobinePrincipaleUI::SetTimeSeriesColors(QColor color) {
    if (timeseries_plot_.graph(0)) {
        auto graph = timeseries_plot_.graph(0);
        QColor linecolor(color);
        QPen linepen(linecolor);
        linepen.setWidth(2);
        QColor brushcolor(color);
        brushcolor.setAlphaF(0.3);
        graph->setPen(linepen);
        graph->setBrush(QBrush(brushcolor));
        timeseries_plot_.replot();
    } else {
        qWarning() << "BobinePrincipaleUI::SetTimeSeriesColors No graph available";
    }
}

void BobinePrincipaleUI::UpdateTimeSeries(double value) {
    if (timeseries_plot_.graph(0)) {
        auto graph = timeseries_plot_.graph(0);
        const QDateTime NOW = QDateTime::currentDateTime();
        graph->data()->removeBefore(NOW.toTime_t() - TIMESERIES_RANGE_SECONDS);
        graph->addData(NOW.toTime_t(), value);
        timeseries_plot_.xAxis->rescale();
        timeseries_plot_.replot();
    } else {
        qWarning() << "BobinePrincipaleUI::UpdateTimeSeries No graph";
    }
}

void BobinePrincipaleUI::Show(bool flag) {
    flag ? connected_->Enable() : connected_->Disable();
    io_load_->SetEnabled(flag);
    flag ? switched_on_->Enable() : switched_on_->Disable();
    commanded_value_->setEnabled(flag);
    flag ? state_->Enable() : state_->Disable();
    flag ? ontarget_->Enable() : ontarget_->Disable();
    actual_value_->setEnabled(flag);
}

}
