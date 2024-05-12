#ifndef ADCGUI_H
#define ADCGUI_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>
#include <QMap>

#include "ADC.h"
#include "ADCRepo.h"

namespace Ui {
class ADCGui;
}

namespace medicyc::cyclotroncontrolsystem::ui::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

class ADCGui : public QWidget
{
    Q_OBJECT

public:
    explicit ADCGui(QWidget *parent = 0);
    ~ADCGui();

public slots:
    void SetCardAddresses();
    void SetPartialScan();
    void ProcessChannelValue(hw_adc::Channel channel, int value);

private slots:
    void ShowPing();
    void ShowConnected();
    void ShowDisconnected();

private:
    void SetupDatabase();
    void SetupLayout();
    void FlashBackground(QLabel *label);
    void SetupPingTimers(std::vector<hw_adc::Channel> channels);
    std::unique_ptr<hw_adc::ADCRepo> repo_;
    std::vector<hw_adc::Channel> GetADCChannels();
    void DisplayUIChannels(const std::vector<hw_adc::Channel>& channels);
    void ReadChannels(const std::vector<hw_adc::Channel>& channels);
    QString FrameStyleSheet(QString name, QString image) const;
    Ui::ADCGui *ui_;
    std::unique_ptr<QSettings> settings_;
    hw_adc::ADC adc_;
    enum class ADC_COL : int { CHANNEL = 0, NAME = 1, VALUE_BINARY = 2, VALUE_ADC = 3, VALUE_PHYSICAL = 4, UNIT_PHYSICAL = 5, READ_BUTTON = 6, AUTO_BUTTON = 7, SPACER = 8 };
    const int NCOL = 9;
    QGridLayout *gridLayout_channels;
    QMap<QString, QTimer*> ping_timers_;
};

}; // namespace adc

#endif
