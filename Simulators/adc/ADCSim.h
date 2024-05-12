#ifndef ADCSIM_H
#define ADCSIM_H

#include <QWidget>
#include <vector>
#include <QSettings>
#include <QLabel>
#include <QGridLayout>

#include "VirtualADC.h"
#include "Channel.h"
#include "ADCRepo.h"

namespace Ui {
class ADCSim;
}

namespace medicyc::cyclotroncontrolsystem::simulators::adc {

namespace hw_adc = medicyc::cyclotroncontrolsystem::hardware::adc;

class ADCSim : public QWidget
{
    Q_OBJECT

public:
    explicit ADCSim(VirtualADC *adc, QWidget *parent = 0);
    ~ADCSim();

public slots:
    void ReadRequest(QString name);

signals:
    void SIGNAL_ReplyToReadRequest(QString name, double physical_value, QString unit);

private slots:

private:
    void SetupDatabase();
    void SetupLayout();
    void FlashBackground(QLabel *label);
    std::unique_ptr<hw_adc::ADCRepo> repo_;
    std::vector<hw_adc::Channel> GetADCChannels();
    void DisplayUIChannels(const std::vector<hw_adc::Channel>& channels);
    QString FrameStyleSheet(QString name, QString image) const;
    Ui::ADCSim *ui_;
    std::unique_ptr<QSettings> settings_;
    VirtualADC* adc_;
    enum class ADC_COL : int { CHANNEL = 0, NAME = 1, VALUE_PHYSICAL = 2, UNIT_PHYSICAL = 3, SPACER = 4 };
    const int NCOL = 8;
    QGridLayout *gridLayout_channels;
    bool connected_ = false;
};

}

#endif
