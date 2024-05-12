#ifndef BOBINEPRINCIPALEUI_H
#define BOBINEPRINCIPALEUI_H

#include <QString>
#include <QFont>
#include <QTimer>
#include <QWidget>

#include "ISubSystemUI.h"
#include "qcustomplot.h"
#include "bobineprincipalecycler_interface.h"
#include "nsinglecontroller_interface.h"
#include "EquipmentGroupValues.h"
#include "Enumerations.h"

class QWidget;
class QPushButton;
class QGridLayout;
class QHBoxLayout;
class QLabel;
class GenericDisplayButton;
class PoleLevelIndicator;

namespace medicyc::cyclotroncontrolsystem::ui::main {

namespace middlelayer = medicyc::cyclotron::middlelayer;

class BobinePrincipaleUI : public QWidget, public ISubSystemUI
{
    Q_OBJECT
    Q_INTERFACES(medicyc::cyclotroncontrolsystem::ui::main::ISubSystemUI)
public:
    BobinePrincipaleUI(QWidget *parent = 0);
    ~BobinePrincipaleUI();
    EquipmentGroupValues GetGroupValues() const;

public slots:
    void Configure(QString name) override;
    void Startup() override;
    void Shutdown() override;
    void Interrupt() override;

    void CyclageStarted();
    void CyclageFinished();
    void CyclageInterrupted();
    void ShutdownStarted();

signals:
    void SIGNAL_State_Off() override;
    void SIGNAL_State_Intermediate() override;
    void SIGNAL_State_Ready() override;
    void SIGNAL_State_Unknown() override;
    void SIGNAL_StartupFinished() override;
    void SIGNAL_ShutdownFinished() override;

private slots:
    void Ping();
    void Show(bool flag);
    void UpdateTimeSeries(double value);
    void StartupAfterConfirmation();
    void ShutdownAfterConfirmation();

private:
    QGridLayout* SetupStatusBar();
    QHBoxLayout* SetupCommandBar();
    void SetupTimeseriesPlot();
    void SetTimeSeriesColors(QColor color);
    void SetupNSingleController();

    QTimer ping_timer_;

    medicyc::cyclotron::NSingleControllerInterface controller_;
    middlelayer::BobinePrincipaleCyclerInterface interface_;
    GenericDisplayButton* connected_;
    PoleLevelIndicator* io_load_;
    GenericDisplayButton* switched_on_;
    GenericDisplayButton* state_;
    GenericDisplayButton* ontarget_;
    QLabel *name_;
    QLabel *commanded_value_;
    QLabel *actual_value_;

    QCustomPlot timeseries_plot_;
    const int TIMESERIES_RANGE_SECONDS = 1800;
};

}

#endif
